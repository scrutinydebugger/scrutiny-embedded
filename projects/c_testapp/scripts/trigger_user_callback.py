from scrutiny.sdk.client import ScrutinyClient

def main():
    client = ScrutinyClient()
    with client.connect('localhost', 8765):
        print(client.user_command(0, bytes([1,2,3,4,5])))

if __name__ == '__main__':
    main()
    