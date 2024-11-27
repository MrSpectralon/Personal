# Ngnix Subdomain Proxy Generator
import argparse
import re

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument("-d", "--domain", help="Domain name you want to proxy. Example: gitlab.tanvgs.no")
arg_parser.add_argument("-i", "--ipv4", help="What server ipv4 you want to proxy towards.")
arg_parser.add_argument("-p", "--port", help="Port you want to proxy towards")
arg_parser.add_argument("-l", "--location", help="Alternative save location for the config file.\n\tDefault location: /etc/nginx/conf.d/")
arg_parser.add_argument("-f", "--file", help="""File - a file listing domains, ip addresses and ports. Added to make multiple simultaious additions possible.
\tFile structure:\n
\t\t[Domain, IP, Port] ->  example2.domain.com 172.16.10.11 80
""")

args = arg_parser.parse_args()

def main():
    global path
    path = "/etc/nginx/conf.d/"
    if args.location:
        if not re.match(r"^(\/[a-zA-Z0-9_]+)+$", args.location):
            print(f"{args.location} is not a valid file path. Hint: start with root folder.")
            return
        path = args.location
        
    
    if args.file:
        from_file(args.file)
        return
    
    if not validate_parameters(args.domain, args.ipv4, args.port):
        print("Use flag '-h' for help.")
        return
    content = make_content(args.domain, args.ipv4, args.port)
    make_file(args.domain, content, path)


def validate_parameters(domain, ip, port) -> bool:
    type = True
    if not valid_domain(domain):
        type = False
    
    if not valid_ip(ip):
        type = False
    
    if not valid_port(port):
        type = False
        
    return type


def valid_domain(domain) -> bool:
    if not domain:
        print("No domain inserted, you need to use the '-d' flag to specify the domain you want to forward use proxy on.")
        return False
    if not re.match(r"^(?:(?:[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?\.)+[a-zA-Z]{2,}|localhost)$", domain):
        print(f"{domain} is not a valid domain name.")
        return False
    return True


def valid_ip(ip:str) -> bool:
    
    if not ip:
        print("No IPv4 inserted, you need to use the '-i' flag to specify the ip you want to forward to.")
        return False
    _bytes = ip.split(".")
    
    if len(_bytes) != 4:
        print(f"{ip} does not contain 4 bytes.")
        return False
    
    for byte in _bytes:
        try:
            byte = int(byte)
            if byte > 254 or byte < 1:
                print(f"Byte {byte} in {ip} is not within the valid IPv4 range of [1-254].")
                return False
        except:
            print(f"{byte} is not a number.")
            return False
    return True
    

def valid_port(port) -> bool:
    if not port:
        print("No port inserted, you need to use the '-p' flag to specify the port you want to forward to.")
        return False
    try:
        port = int(port)
        if port > 65535 or port < 1:
             print(f"Port '{port}' cannot be larger than 65535 or lower than 1.")
             return False
        
    except:
        print(f"Port '{port}' is not a number.")
        return False
    return True


def from_file(file):
    param_file = open(file, "r")
    lines = param_file.readlines()
    for line in lines:
        if line[0] == "#":
            continue
        params = line.split()
        if params:
            if not validate_parameters(params[0], params[1], params[2]):
                print(f"Not valid input format on  line:\n\t{line}\nIn file {file}.")
                print(f"Skipping line.")
                continue
            file_content = make_content(params[0], params[1], params[2])     
            make_file(params[0], file_content, path)


def make_content(server_name, ip, port):
    http_1 = """server {
    listen 80;
    listen [::]:80;
"""
    serverName = f"\tserver_name {server_name} www.{server_name};\n"
    http_2 = "\treturn 302 https://$server_name$request_uri;\n}"
    
    https_1 = """
server {
    # SSL configuration
    listen 443 ssl http2;
    listen [::]:443 ssl http2;
    ssl_certificate         /etc/ssl/cert.pem;
    ssl_certificate_key     /etc/ssl/key.pem;
"""
    
    location = """
    location / {
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;\n"""
    
    if port:
        proxy = f'\t\tproxy_pass "http://{ip}:{port}";'
    else:
        proxy = f'\t\tproxy_pass {ip}'
    https_2 = "\n\t}\n}"
    
    return http_1 + serverName + http_2 + https_1 + serverName + location + proxy + https_2


def make_file(server_name, content, path):
    if path[-1] != "/":
        path += "/"
    try:
        f = open(path + server_name + ".conf", "x")
        f.write(content)
        f.close()
    except FileExistsError:
        print("######  Warning!  ######")
        print(f"The file '{path + server_name}.conf' already exists. Do you want to replace it? Y/N")
        while True:
            i = input().lower()
            if i == "n":
                print("Aborting write operation")
                return
            elif i == "y":
                print(f"Overwriting content in file {path + server_name}.conf")
                f = open(path + server_name + ".conf", "w")
                f.write(content)
                f.close()
                return
            print("No such option. Valid options are Y/N -> (Y)es / (N)o")
            
            
if __name__ == "__main__":
    main()
