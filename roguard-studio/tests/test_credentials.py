from app.credentials import integration_token


def test_studio_local_credential_file_is_optional_and_environment_wins(tmp_path,monkeypatch):
    monkeypatch.delenv('STUDIO_INTEGRATION_TOKEN',raising=False)
    (tmp_path/'.env').write_text('IGNORED=private\nSTUDIO_INTEGRATION_TOKEN="'+('f'*40)+'"\n')
    assert integration_token(tmp_path)=='f'*40
    assert integration_token(tmp_path,allow_file=False)==''
    monkeypatch.setenv('STUDIO_INTEGRATION_TOKEN','e'*40)
    assert integration_token(tmp_path)=='e'*40
    monkeypatch.setenv('STUDIO_INTEGRATION_TOKEN','')
    assert integration_token(tmp_path)=='' # Explicit disable is honored.


def test_studio_restart_probe_reuses_a_closing_port_without_claiming_a_live_listener():
    import importlib.util
    import socket
    from pathlib import Path
    script=Path(__file__).resolve().parents[1]/'scripts/start.py'
    spec=importlib.util.spec_from_file_location('studio_start_probe',script)
    launcher=importlib.util.module_from_spec(spec);spec.loader.exec_module(launcher)
    with socket.socket() as listener:
        listener.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        listener.bind(('127.0.0.1',0));listener.listen()
        port=listener.getsockname()[1]
        assert launcher.free_port(port)!=port
        client=socket.create_connection(('127.0.0.1',port),timeout=2)
        accepted,_=listener.accept();accepted.close()
        assert client.recv(1)==b''
        client.close()
    assert launcher.free_port(port)==port
