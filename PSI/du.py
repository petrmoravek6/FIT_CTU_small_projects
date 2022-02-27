class KeyPair:
    def __init__(self, server_key, client_key):
        self.server_key = server_key
        self.client_key = client_key


key_dict = {
    0: KeyPair(23019, 32037)
}

client_name = 'Oompa Loompa'
server_id = 0


def get_hash_from_name(client_name):
    ascii_value = 0
    for c in client_name:
        ascii_value += ord(c)
    return (ascii_value * 1000) % 65536


name_hash = get_hash_from_name(client_name)
print(f'Hash from the name is: {name_hash}')
