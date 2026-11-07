import time
import urllib.request

VM = "http://127.0.0.1:8428/api/v1/import/prometheus"
FILES = [
    "/tmp/oai_ue_counters.json",
    "/tmp/oai_gnb_counters.json",
]
NAMES = [
    "rrc_connected",
    "nas_registration_accept",
    "gnb_ng_setup_response",
    "gnb_rrc_setup_complete",
]


def last_value(text, name):
    if f'"{name}"' not in text:
        return None
    part = text.rsplit(f'"{name}"', 1)[-1]
    for word in part.replace(",", " ").replace("}", " ").split():
        if word.isdigit():
            return int(word)
    return None


while True:
    metrics = {}
    for path in FILES:
        try:
            text = open(path).read()
        except FileNotFoundError:
            continue
        for name in NAMES:
            v = last_value(text, name)
            if v is not None:
                metrics[name] = v

    if metrics:
        body = "\n".join(f"{k} {v}" for k, v in metrics.items()) + "\n"
        try:
            req = urllib.request.Request(VM, data=body.encode(), method="POST")
            urllib.request.urlopen(req, timeout=5)
            print("отправили:", metrics)
        except Exception as e:
            print("ошибка:", e)
    else:
        print("нет данных, ждём...")

    time.sleep(5)