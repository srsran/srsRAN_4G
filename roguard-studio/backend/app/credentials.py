"""Load only the integration credential from Studio's private local file."""
import os
import shlex


def integration_token(studio, allow_file=True):
    if 'STUDIO_INTEGRATION_TOKEN' in os.environ:
        return os.environ['STUDIO_INTEGRATION_TOKEN']
    path=studio/'.env'
    if not allow_file or not path.exists():
        return ''
    token=''
    for line in path.read_text().splitlines():
        key,separator,raw=line.strip().removeprefix('export ').partition('=')
        if separator and key.strip()=='STUDIO_INTEGRATION_TOKEN':
            token=' '.join(shlex.split(raw,comments=True))
    return token
