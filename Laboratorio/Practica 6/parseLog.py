import matplotlib.pyplot as plt


def extractArrayData(file) -> ([], []):
    data = []
    loggedValues = []
    temp = ""
    counter = 0
    f = open(file, "r")
    for x in f:
        if not counter:
            data.append(temp.replace("\n", ""))
            temp = ""
        counter = not counter
        temp = temp + (x.strip().split(":")[1])
        if len(x.strip()) > 1:
            loggedValues.append(int(x.strip().split(":")[1], 16))
    data.append(temp.replace("\n", ""))
    f.close()
    data.pop(0)  # Remove first element (empty)
    return data, loggedValues


def ArrayStringToInt(array, base) -> []:
    _dataInt = []
    for int16b in array:
        _dataInt.append(int(int16b, base))
    return _dataInt


def WriteByteArray(logValues, filename):
    loggedBytes = bytearray(logValues)
    with open(filename, 'wb') as fpOutput:
        fpOutput.write(loggedBytes)


# Extraccion de arreglo hexadecimal y logged (valores hexadecimales de 1 byte)
int15_t, logged = extractArrayData("test.log")
dataInt = ArrayStringToInt(int15_t, 16)  # Conversion de arreglo hexadecimal a decimal
arrayHistogram = plt.hist(dataInt, bins=1024)  # bins = cantidad de contenedores que utilizara la grafica
plt.title("Histogram of Logged Output")
plt.savefig("histogram.jpg", dpi=400)
WriteByteArray(logged, 'output.bin')
