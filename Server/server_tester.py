import requests

url = "http://127.0.0.1:8080/health"
url = "http://jserv.ddns.net:8080/info"
url2 = "http://testserver.aeq-web.com/sim800_test/"
data = {
    'Sensor0': 100,
    'Sensor1': 200  
}

response = requests.get(url, json=data)

print(f"Status Code: {response.status_code}")
print(f"Response Text: {response.text}")
# print(f"{response=}")