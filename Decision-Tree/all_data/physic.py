import csv
from matplotlib import pyplot as plt
import os
import numpy as np
I = []
U = []
def extractFromCsv(filename):
    with open(filename,'r') as f:
        reader = csv.reader(f)
        for row in reader:
            if reader.line_num == 1:        #跳过表头
                continue
            U.append(float(row[0]))
            I.append((float(row[1])))

def drawPic(filename):
    fig = plt.figure(dpi=256, figsize=(10, 8))
    plt.plot(U,I,'r*')
    plt.xlabel('U(V)',fontsize=10)
    plt.ylabel('I(10e-9A)',fontsize=10)
    plt.title('The line chart of the relation between t and Velocity',fontsize=15)
    plt.ylim(0,max(I)+0.5)

    x = np.linspace(0,max(I)+0.5,40)
    plt.yticks(x,fontsize=6)

    if not os.path.exists(".\\graph"):
        os.mkdir('.\\graph')
    plt.savefig('.\\graph\\{}.jpg'.format(filename))

if __name__ == "__main__":
    filename='phy.csv'
    extractFromCsv(filename)
    drawPic(filename)

