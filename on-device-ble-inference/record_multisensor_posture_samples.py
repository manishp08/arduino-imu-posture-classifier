import serial
import csv

# Set the serial port name (check your Arduino IDE for the correct port)
# serial_port = '/dev/cu.usbmodem21301'  # serial port for linux or MAC
com_port = 'COM3' # com port for Windows
baud_rate = 9600
threshold = 0.1

try:
    # Open the serial port connection

    # serial port for linux or MAC
    # ser = serial.Serial(serial_port, 9600)
    ser = serial.Serial(port = com_port, baudrate = baud_rate) # com port for Windows

    # Open a CSV file for writing sensor data with column names for each state
    data_sheet_csv_file = open('multisensor_posture_samples.csv', 'w', newline='')

    column_names = ['Ax','Ay','Az','Gx','Gy','Gz','Mx','My','Mz','labels']  # Specify column names here

    supine = int(1)
    prone = int(2)
    side = int(3)
    sitting = int(4)
    unknown = int(5)

    data_sheet_writer = csv.writer(data_sheet_csv_file)
    data_sheet_writer.writerow(column_names)

    while True:
        # Read a line of data from the serial port
        data = ser.readline().decode('utf-8').strip()

        # Print the received data for debugging
        print("Received data:", data)

        # Split the data and parse it (modify this part as needed)
        values = data.split('\t')
        if len(values) >= 9:
            ax, ay, az, gx, gy, gz, mx, my, mz = [float(val.split(':')[1]) for val in values[:9]]
            
            # Print parsed values for debugging
            print("Parsed values: Ax={}, Ay={}, Az={}, Gx={}, Gy={}, Gz={}, Mx={}, My={}, Mz={}".format(ax, ay, az, gx, gy, gz, mx, my, mz))

            # Write the data to the appropriate CSV file based on 'ay' value
            if (az > ax and az > ay and az <= 1+threshold and az >= 1-threshold):
                print('supine')
                data_sheet_writer.writerow([ax, ay, az, gx, gy, gz, mx, my, mz, supine])
                data_sheet_csv_file.flush()  # Ensure data is written immediately to the file
            elif (az < ax and az < ay and az <= -1+threshold and az >= -1-threshold):
                print('prone')
                data_sheet_writer.writerow([ax, ay, az, gx, gy, gz, mx, my, mz, prone])
                data_sheet_csv_file.flush()  # Ensure data is written immediately to the file
            elif ((ay > ax and ay > az and ay >= 1-threshold and ay <= 1+threshold) or (ay < ax and ay < az and ay >= -1-threshold and ay <= -1+threshold)):
                print('side')
                data_sheet_writer.writerow([ax, ay, az, gx, gy, gz, mx, my, mz, side])
                data_sheet_csv_file.flush()  # Ensure data is written immediately to the file
            elif((ax > ay and ax > az and ax >= 1-threshold and ax <= 1+threshold) or (ax < ay and ax < az and ax >= -1-threshold and ax <= -1+threshold)):
                print('sitting')
                data_sheet_writer.writerow([ax, ay, az, gx, gy, gz, mx, my, mz, sitting])
                data_sheet_csv_file.flush()  # Ensure data is written immediately to the file   
            else:
                print('unknown')
                data_sheet_writer.writerow([ax, ay, az, gx, gy, gz, mx, my, mz, unknown])
                data_sheet_csv_file.flush()  # Ensure data is written immediately to the file 

except KeyboardInterrupt:
    pass
except Exception as e:
    print("An error occurred:", str(e))

finally:
    # Close the CSV file and serial port
    data_sheet_csv_file.close()
    ser.close()