# Certificates

1. Drop the certificates downloaded from AWS here, in PEM format:
- `XXX-certificate.pem.crt`
- `XXX-private.pem`

2. Run `python3 ./script/gencertsh.py` from project root directory.

3. Following files should have been created in `src/certs` directory:
- CA: `ca_cert.cer`
- Public certificate: `public_cert.cer`
- Private key: `private_key.cer`