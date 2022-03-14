import os

def text_to_string(filename: str, varname: str = "certificate_pem"):
    out = "\""
    with open(filename, "r+") as fp:
        out += "\\r\\n\"\n\"".join(line.strip() for line in fp.readlines())
    out += "\\r\\n\"\n"
    return out

def list_files(directory: str, extension: str = "-certificate.pem.crt"):
    return [os.path.join(directory, f) for f in os.listdir(directory) if f.endswith(extension)]

def get_device_public_certificate(directory: str):
    files = list_files(directory, "-certificate.pem.crt")

    if len(files) > 0:
        return files[0]

def get_device_private_key(directory: str):
    files = list_files(directory, "-private.pem.key")

    if len(files) > 0:
        return files[0]

def generate_certificates_headers(directory: str = "./certs"):
    certificates = {
        "public_cert": get_device_public_certificate(directory),
        "private_key": get_device_private_key(directory),
        "ca_cert": os.path.join(directory, "AmazonRootCA1.pem")
    }

    assert all(certificates.values())

    for name, certfile in certificates.items():
        filename = f"./src/certs/{name}.cer"
        with open(filename, "w+") as fp:
            print(f"Generating {filename}")
            fp.write(text_to_string(certfile, name))

if __name__ == "__main__":
    generate_certificates_headers()