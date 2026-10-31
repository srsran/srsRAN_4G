import os
from datetime import datetime, timezone
from pathlib import Path
from sqlalchemy import create_engine, Column, String, Integer, JSON, select, event, insert, update, func
from sqlalchemy.orm import DeclarativeBase, Session
from uuid import uuid4
from .integration_contract import project, SCHEMA, compatible_runtime

def now():
    return datetime.now(timezone.utc).isoformat(timespec='milliseconds').replace('+00:00','Z')

class Base(DeclarativeBase): pass
class Record(Base):
    __tablename__ = 'records'
    key = Column(String, primary_key=True)
    kind = Column(String, index=True, nullable=False)
    value = Column(JSON, nullable=False)

class IntegrationRecord(Base):
    __tablename__ = 'integration_events_v1'
    __table_args__ = {'sqlite_autoincrement': True}
    sequence = Column(Integer, primary_key=True, autoincrement=True)
    kind = Column(String, index=True, nullable=False)
    run_id = Column(String, index=True)
    value = Column(JSON, nullable=False)

class Store:
    def __init__(self, directory: Path):
        directory.mkdir(parents=True, exist_ok=True, mode=0o700)
        os.chmod(directory,0o700)
        self.engine = create_engine(f'sqlite:///{directory / "studio.sqlite3"}', connect_args={'check_same_thread':False})
        @event.listens_for(self.engine,'connect')
        def configure(dbapi, _):
            dbapi.execute('PRAGMA journal_mode=WAL')
            dbapi.execute('PRAGMA busy_timeout=5000')
        Base.metadata.create_all(self.engine)
        (directory/'studio.sqlite3').chmod(0o600)
        # One durable source identity and atomic bootstrap of pre-integration evidence.
        with self.engine.connect() as c:
            c.exec_driver_sql('BEGIN IMMEDIATE')
            identity = c.execute(select(Record.value).where(Record.key=='integration:source')).scalar_one_or_none()
            if identity is None:
                identity = {'id': str(uuid4())}
                c.execute(insert(Record).values(key='integration:source', kind='integration', value=identity))
            self.source_id = identity['id']
            if c.execute(select(Record.key).where(Record.key=='integration:bootstrapped')).first() is None:
                records = c.execute(select(Record).where(Record.kind.in_(['run','metric','event','topology']))).mappings().all()
                records = sorted(records, key=lambda r: r['value'].get('timestamp') or r['value'].get('started_at') or '')
                for record in records:
                    self._journal(c, record['kind'], record['value'])
                c.execute(insert(Record).values(key='integration:bootstrapped', kind='integration', value={'version': 1}))
            c.commit()
        # Additive v1 enrichment: re-announce current applied identities for old journals.
        with self.engine.connect() as c:
            c.exec_driver_sql('BEGIN IMMEDIATE')
            if c.execute(select(Record.key).where(Record.key=='integration:applied-identities-v1')).first() is None:
                runs=c.execute(select(Record.value).where(Record.kind=='run')).scalars().all()
                if runs:
                    latest=max(runs, key=lambda r:r.get('started_at') or '')
                    self._journal(c, 'run', latest)
                c.execute(insert(Record).values(key='integration:applied-identities-v1',kind='integration',value={'version':1}))
            c.commit()

    def _projection(self, connection, kind, value):
        topology_id = None
        component_id = value.get('component_id')
        # Older Studio audit rows carried their run identity inside the observation.
        run_id = value.get('run_id') or (value.get('observed') or {}).get('run_id')
        if run_id:
            run = connection.execute(select(Record.value).where(Record.key=='run:'+run_id)).scalar_one_or_none()
            if run:
                topology_id = run['topology']['id']
                role=value.get('component')
                if role in ('ue','enb','epc'):
                    nodes=[n for n in run['topology']['nodes'] if n['kind'] in (('enb','temporary','suspicious') if role=='enb' else (role,))]
                    if len(nodes)==1 and not component_id:component_id=nodes[0]['id']
            value = {**value, 'run_id': run_id}
        result=project(kind, value, topology_id)
        if result is not None:result['component_id']=component_id
        return result

    def _journal(self, connection, kind, value, old=None):
        payload = self._projection(connection, kind, value)
        if payload is None or (old is not None and payload == self._projection(connection, kind, old)):
            return
        payload['timestamp'] = payload.get('timestamp') or now()
        connection.execute(insert(IntegrationRecord).values(kind=payload['kind'], run_id=payload['run_id'], value=payload))

    def envelope(self, row):
        value = row.value
        if value.get('kind') == 'runtime':
            value = {**value, 'data':compatible_runtime(value['data'])}
        return {'schema_version': SCHEMA, 'source_id': self.source_id,
                'sequence': row.sequence, 'event_id': f'{self.source_id}:{row.sequence}',
                'timestamp_semantics': 'ingestion_utc', **value}

    def integration_head(self):
        with Session(self.engine) as s:
            return s.scalar(select(func.max(IntegrationRecord.sequence))) or 0

    def integration_events(self, after, limit=200):
        with Session(self.engine) as s:
            return [self.envelope(r) for r in s.scalars(select(IntegrationRecord).where(IntegrationRecord.sequence>after).order_by(IntegrationRecord.sequence).limit(limit))]

    def integration_snapshot(self):
        # Explicit read transaction: watermark and state share the same SQLite snapshot.
        with Session(self.engine) as s:
            s.connection().exec_driver_sql('BEGIN')
            watermark = s.scalar(select(func.max(IntegrationRecord.sequence))) or 0
            runtime = s.scalar(select(IntegrationRecord).where(IntegrationRecord.kind=='runtime').order_by(IntegrationRecord.sequence.desc()).limit(1))
            metrics = []
            if runtime:
                for component in ('ue','enb'):
                    row = s.scalar(select(IntegrationRecord).where(IntegrationRecord.kind=='metric', IntegrationRecord.run_id==runtime.run_id, IntegrationRecord.value['data']['component'].as_string()==component).order_by(IntegrationRecord.sequence.desc()).limit(1))
                    if row:
                        metrics.append(self.envelope(row))
            return {'schema_version': SCHEMA, 'source_id': self.source_id, 'timestamp': now(),
                    'watermark': watermark, 'cursor': f'{self.source_id}:{watermark}',
                    'runtime': self.envelope(runtime) if runtime else None, 'metrics': metrics,
                    'capabilities': {'native_enodebs': 1, 'native_ues': 1, 'observed_parameters': ['pci','earfcn'], 'multi_cell_verified': False}}
    def get(self, kind, key, default=None):
        with Session(self.engine) as s:
            r = s.get(Record,f'{kind}:{key}')
            return r.value if r else default
    def put(self, kind, key, value):
        # Source mutation and journal entry commit together. No queue loss window.
        with self.engine.connect() as c:
            c.exec_driver_sql('BEGIN IMMEDIATE')
            record_key = f'{kind}:{key}'
            old = c.execute(select(Record.value).where(Record.key==record_key)).scalar_one_or_none()
            if old is None:
                c.execute(insert(Record).values(key=record_key, kind=kind, value=value))
            else:
                c.execute(update(Record).where(Record.key==record_key).values(value=value))
            self._journal(c, kind, value, old)
            c.commit()
        return value
    def list(self, kind):
        with Session(self.engine) as s:
            return [r.value for r in s.scalars(select(Record).where(Record.kind==kind).order_by(Record.key))]
    def save_topology(self, value):
        key='topology:'+value['id']
        # Acquire the SQLite write lock before checking revisions; concurrent windows cannot lose updates.
        with self.engine.connect() as connection:
            connection.exec_driver_sql('BEGIN IMMEDIATE')
            old=connection.execute(select(Record.value).where(Record.key==key)).scalar_one_or_none()
            if old is not None and old['revision']!=value['revision']:
                connection.rollback()
                raise ValueError('Topology changed in another window; reload before saving')
            saved={**value,'revision':value['revision']+1}
            if old is None:connection.execute(insert(Record).values(key=key,kind='topology',value=saved))
            else:connection.execute(update(Record).where(Record.key==key).values(value=saved))
            self._journal(connection, 'topology', saved, old)
            connection.commit()
            return saved
    def delete(self, kind, key):
        with Session(self.engine) as s:
            r=s.get(Record,f'{kind}:{key}')
            if r: s.delete(r); s.commit()
