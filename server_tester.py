import requests

url = "http://jserv.ddns.net:8080"
data = {
    'Sensor0': 100,
    'Sensor1': 200
}

response = requests.post(url, json=data)

print(f"Status Code: {response.status_code}")
print(f"Response Text: {response.text}")