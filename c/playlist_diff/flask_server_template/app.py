import os
import requests
from flask import Flask, redirect, request
from urllib.parse import urlencode
from dotenv import load_dotenv

load_dotenv('../hidden_from_git/.env')

app = Flask(__name__)

GOOGLE_CLIENT_ID = os.getenv("GOOGLE_CLIENT_ID")
GOOGLE_CLIENT_SECRET = os.getenv("GOOGLE_CLIENT_SECRET")
REDIRECT_URI = "http://localhost:8080/callback"
DISCOVERY_URL = "https://accounts.google.com/.well-known/openid-configuration"

def get_google_config():
    return requests.get(DISCOVERY_URL).json()

@app.route("/")
def index():
    return handle_redirect()

@app.route("/login")
def login():
    return handle_redirect()

def handle_redirect():
    cfg = get_google_config()
    auth_endpoint = cfg["authorization_endpoint"]
    params = {
        "client_id": GOOGLE_CLIENT_ID,
        "redirect_uri": REDIRECT_URI,
        "scope": "https://www.googleapis.com/auth/youtube",
        "response_type": "code",
        "access_type": "online",
        "prompt": "consent"
    }

    return redirect(auth_endpoint + "?" + urlencode(params))


@app.route("/callback")
def callback():
    code = request.args.get("code")
    cfg = get_google_config()
    token_endpoint = cfg["token_endpoint"]

    token_res = requests.post(
        token_endpoint,
        data={
            "code": code,
            "client_id": GOOGLE_CLIENT_ID,
            "client_secret": GOOGLE_CLIENT_SECRET,
            "redirect_uri": REDIRECT_URI,
            "grant_type": "authorization_code"
        },
        headers={"Content-Type": "application/x-www-form-urlencoded"}
    )
    oneliner = ""
    for line in token_res.content.decode('utf-8').split('\n'):
        oneliner += line 
    with open("../reply.json", mode='w+') as file:
        file.write(oneliner)
    return f"<pre>You can close this window now.</pre>"  


if __name__ == "__main__":
    app.run(debug=True, port=8080)

