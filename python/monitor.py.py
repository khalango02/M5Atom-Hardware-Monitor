import requests
import time

ATOM_IP = "192.168.1.12"

LHM_URL = "http://localhost:8085/data.json"


def walk(node, results):

    text = node.get("Text", "")
    value = node.get("Value", "")

    if text == "CPU Total":
        results["cpu_usage"] = value

    elif text == "Core (Tctl/Tdie)":
        results["cpu_temp"] = value

    elif text == "GPU Core":

        if "%" in str(value):
            results["gpu_usage"] = value

        elif "°C" in str(value):
            results["gpu_temp"] = value

    elif text == "Memory" and "%" in str(value):
        results["ram_usage"] = value

    for child in node.get("Children", []):
        walk(child, results)


while True:

    try:

        data = requests.get(LHM_URL, timeout=2).json()

        metrics = {}

        walk(data, metrics)

        payload = {
            "cpu_usage": float(
                metrics["cpu_usage"]
                .replace("%", "")
                .replace(",", ".")
            ),

            "cpu_temp": float(
                metrics["cpu_temp"]
                .replace("°C", "")
                .replace(",", ".")
            ),

            "gpu_usage": float(
                metrics["gpu_usage"]
                .replace("%", "")
                .replace(",", ".")
            ),

            "gpu_temp": float(
                metrics["gpu_temp"]
                .replace("°C", "")
                .replace(",", ".")
            ),

            "ram_usage": float(
                metrics["ram_usage"]
                .replace("%", "")
                .replace(",", ".")
            )
        }

        print(payload)

        requests.post(
            f"http://{ATOM_IP}/update",
            json=payload,
            timeout=2
        )

    except Exception as e:
        print(e)

    time.sleep(1)