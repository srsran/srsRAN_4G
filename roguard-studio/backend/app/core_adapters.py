"""Core lifecycle policy. External services are never implicitly adopted or killed."""
from dataclasses import dataclass
import shutil

@dataclass(frozen=True)
class CoreAdapter:
    id: str
    ownership: str
    handover_verified: bool
    dependencies: tuple[str, ...] = ()

    def diagnostic(self):
        missing = [name for name in self.dependencies if not shutil.which(name)]
        return {'id': self.id, 'ownership': self.ownership,
                'handover_verified': self.handover_verified, 'missing_dependencies': missing,
                'execution_implemented': self.id == 'srsepc',
                'status': 'implemented' if self.id == 'srsepc' else 'blocked by external-core provisioning and lifecycle integration',
                'reason': None if self.id == 'srsepc' else
                    'Open5GS requires an isolated MME/SGW/SMF/UPF, MongoDB subscriber provisioning and owned network setup. No existing external core will be controlled.'}

CORES = {
    'srsepc': CoreAdapter('srsepc', 'run-owned native process', False),
    'open5gs': CoreAdapter('open5gs', 'not adopted; connection/start disabled pending native acceptance', False,
                           ('open5gs-mmed', 'open5gs-sgwcd', 'open5gs-sgwud', 'open5gs-smfd', 'open5gs-upfd', 'mongod')),
}
