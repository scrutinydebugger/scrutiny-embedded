from scrutiny.sdk.client import ScrutinyClient
import time

def main():
    client = ScrutinyClient()
    with client.connect('localhost', 8765):
        v = client.watch("/rpv/x3000")
        client.wait_new_value_for_all()

        while True:
            print(v.value)
            v.value += 1
            time.sleep(0.2)

if __name__ == '__main__':
    main()
    