"""Dedicated local browser-test services using the actual Studio and Roguard apps.

The small dashboard proxy implements the existing server-side credential boundary;
its upstream is the real Roguard FastAPI app/Bridge/Store, not fabricated responses.
User services, credentials and databases are never used. Stop with Ctrl+C.
"""
import argparse
import asyncio
from contextlib import asynccontextmanager,contextmanager
import json
import os
from pathlib import Path
import secrets
import signal
import socket
import sys
import httpx
import uvicorn
from fastapi import FastAPI,Request,HTTPException
from fastapi.responses import JSONResponse

async def main():
    p=argparse.ArgumentParser();p.add_argument('--data-dir',type=Path,required=True);p.add_argument('--studio-port',type=int,default=18280);p.add_argument('--frontend-port',type=int,default=13280);p.add_argument('--dashboard-port',type=int,default=13282)
    args=p.parse_args();os.umask(0o077)
    for port in (args.studio_port,args.dashboard_port):
        with socket.socket() as listener:listener.bind(('127.0.0.1',port))
    args.data_dir.mkdir(parents=True,exist_ok=True)
    keyfile=args.data_dir/'private-credentials.json'
    if not keyfile.exists():keyfile.write_text(json.dumps({k:secrets.token_urlsafe(40) for k in ('machine','portal','analyst')}));keyfile.chmod(0o600)
    keys=json.loads(keyfile.read_text());studio=Path(__file__).resolve().parents[1]
    os.environ.update(STUDIO_INTEGRATION_TOKEN=keys['machine'],ROGUARD_ANALYST_TOKEN=keys['analyst'],ROGUARD_BACKEND_PORT=str(args.studio_port),ROGUARD_FRONTEND_PORT=str(args.frontend_port))
    sys.path.insert(0,str(studio/'backend'))
    from app.main import create_app
    studio_app=create_app(args.data_dir/'studio')
    consumer=Path(os.environ.get('ROGUARD_REPO',studio.parents[1]/'rogue-bs-detection'))
    sys.path.insert(0,str(consumer/'backend'))
    from simulation_bridge.main import create_app as create_roguard
    saved={k:os.environ.pop(k,None) for k in ('STUDIO_BASE_URL','STUDIO_INTEGRATION_TOKEN','STUDIO_EXPECTED_SOURCE_ID')}
    roguard=create_roguard(data_dir=args.data_dir/'roguard',portal_token=keys['portal'],analyst_token=keys['analyst'])
    os.environ.update({k:v for k,v in saved.items() if v is not None})
    @asynccontextmanager
    async def lifespan(app):
        async with roguard.router.lifespan_context(roguard):yield
    proxy=FastAPI(lifespan=lifespan)
    @proxy.api_route('/api/simulation/connection',methods=['GET','POST'])
    async def connection(request:Request):
        if request.url.hostname not in ('127.0.0.1','localhost'):raise HTTPException(403)
        authorization='Bearer '+keys['portal'] if request.method=='GET' else request.headers.get('authorization','')
        async with httpx.AsyncClient(transport=httpx.ASGITransport(app=roguard),base_url='http://127.0.0.1',headers={'Authorization':authorization}) as client:
            upstream=await client.request(request.method,'/api/v1/simulation/connection',content=await request.body(),headers={'Content-Type':'application/json'})
        return JSONResponse(upstream.json(),status_code=upstream.status_code)
    @proxy.get('/audit/evidence')
    async def evidence(request:Request):
        if not secrets.compare_digest(request.headers.get('authorization',''),'Bearer '+keys['portal']):raise HTTPException(401)
        rows=roguard.state.store.db.execute('SELECT payload FROM raw_events ORDER BY sequence').fetchall()
        metrics=[json.loads(row[0]) for row in rows if json.loads(row[0])['kind']=='metric']
        return {'cursor':roguard.state.store.cursor(),'raw_events':len(rows),
                'native_ue_components':sorted({m['component_id'] for m in metrics if m['data']['component']=='ue' and m['source_type']=='native-measurement'}),
                'native_enb_components':sorted({m['component_id'] for m in metrics if m['data']['component']=='enb'}),
                'rejections':roguard.state.store.db.execute('SELECT COUNT(*) FROM rejections').fetchone()[0]}
    class ManagedServer(uvicorn.Server):
        @contextmanager
        def capture_signals(self):yield
    servers=[ManagedServer(uvicorn.Config(studio_app,host='127.0.0.1',port=args.studio_port,log_level='warning',access_log=False)),ManagedServer(uvicorn.Config(proxy,host='127.0.0.1',port=args.dashboard_port,log_level='warning',access_log=False))]
    def stop():
        for server in servers:server.should_exit=True
    for sig in (signal.SIGINT,signal.SIGTERM):asyncio.get_running_loop().add_signal_handler(sig,stop)
    print(json.dumps({'studio_api':f'http://127.0.0.1:{args.studio_port}','test_dashboard':f'http://127.0.0.1:{args.dashboard_port}','data_dir':str(args.data_dir)}),flush=True)
    await asyncio.gather(*(server.serve() for server in servers))

if __name__=='__main__':asyncio.run(main())
