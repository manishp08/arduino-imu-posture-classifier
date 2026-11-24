import serial
import csv

# Set the serial port name (check your Arduino IDE for the correct port)
# serial_port = '/dev/cu.usbmodem21301'  # serial port for linux or MAC
com_port = 'COM6' # com port for Windows
baud_rate = 9600
threshold = 0.1

try:
    # Open the serial port connection

    # serial port for linux or MAC
    # ser = serial.Serial(serial_port, 9600)
    ser = serial.Serial(port = com_port, baudrate = baud_rate) # com port for Windows

    # Open a CSV file for writing sensor data with column names for each state
    supine_csv_file = open('supine.csv', 'w', newline='')
    prone_csv_file = open('prone.csv', 'w', newline='')
    side_csv_file = open('side.csv', 'w', newline='')

    column_names = ['Ax', 'Ay', 'Az']  # Specify column names here

    supine_writer = csv.writer(supine_csv_file)
    supine_writer.writerow(column_names)
    prone_writer = csv.writer(prone_csv_file)
    prone_writer.writerow(column_names)
    side_writer = csv.writer(side_csv_file)
    side_writer.writerow(column_names)

    while True:
        # Read a line of data from the serial port
        data = ser.readline().decode('utf-8').strip()

        # Print the received data for debugging
        print("Received data:", data)

        # Split the data and parse it (modify this part as needed)
        values = data.split('\t')
        if len(values) >= 3:
            ax, ay, az = [float(val.split(':')[1]) for val in values[:3]]
            
            # Print parsed values for debugging
            print("Parsed values: Ax={}, Ay={}, Az={}".format(ax, ay, az))

            # Write the data to the appropriate CSV file based on 'ay' value
            if (az > ax and az > ay and az <= 1+threshold and az >= 1-threshold):
                print('supine')
                supine_writer.writerow([ax, ay, az])
                supine_csv_file.flush()  # Ensure data is written immediately to the file
            elif (az < ax and az < ay and az <= -1+threshold and az >= -1-threshold):
                print('prone')
                prone_writer.writerow([ax, ay, az])
                prone_csv_file.flush()  # Ensure data is written immediately to the file
            elif ((ay > ax and ay > az and ay >= 1-threshold and ay <= 1+threshold) or (ay < ax and ay < az and ay >= -1-threshold and ay <= -1+threshold)):
                print('side')
                side_writer.writerow([ax, ay, az])
                side_csv_file.flush()  # Ensure data is written immediately to the file
            else:
                print('unkown')

except KeyboardInterrupt:
    pass
except Exception as e:
    print("An error occurred:", str(e))

finally:
    # Close the CSV file and serial port
    supine_csv_file.close()
    prone_csv_file.close()
    side_csv_file.close()
    ser.close()