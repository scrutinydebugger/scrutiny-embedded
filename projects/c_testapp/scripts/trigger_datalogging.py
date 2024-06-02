from scrutiny.sdk.datalogging import *
from scrutiny.sdk.client import ScrutinyClient

def main():
    client = ScrutinyClient()
    with client.connect('localhost', 8765):
        var = client.watch('/rpv/x1000')
        var2 = client.watch('/rpv/x1001')

        config = DataloggingConfig(sampling_rate=0, decimation=1, timeout=0, name="MyGraph")
        config.configure_trigger(TriggerCondition.GreaterThan, [var, 10], position=0.5, hold_time=0)
        config.configure_xaxis(XAxisType.MeasuredTime)
        axis1 = config.add_axis('Axis 1')
        config.add_signal(var, axis1, name="x1000")
        config.add_signal(var2, axis1, name="x1001")
        var.value = 9
        request = client.start_datalog(config)

        timeout = 60
        print(f"Embedded datalogger armed. Waiting for trigger...")
        try:
            var.value = 11
            request.wait_for_completion(timeout)    # Wait for the trigger condition to be fulfilled
        except sdk.exceptions.TimeoutException:
            print(f'Timed out while waiting')

        if request.completed:   # Will be False if timed out
            if request.is_success:
                acquisition = request.fetch_acquisition()
                filename = "my_acquisition.csv"
                acquisition.to_csv(filename)
                print(f"Acquisition [{acquisition.reference_id}] saved to CSV format in {filename}")
            else:
                print(f"The datalogging acquisition failed. Reason: {request.failure_reason}")

if __name__ == '__main__':
    main()