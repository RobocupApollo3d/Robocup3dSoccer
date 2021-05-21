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
            if len(row) == 2:
                U.append(float(row[0]))
                I.append((float(row[1])))

def drawPic(filename):
    fig = plt.figure(dpi=256, figsize=(80, 60))
    plt.plot(U,I,'r*')
    plt.xlabel('Time(t)',fontsize=10)
    plt.ylabel('Velocity(v)',fontsize=10)
    plt.title('The line chart of the relation between Velocity(v) and Time(t)',fontsize=15)

    plt.xlim(min(U),max(U))
    xfen = np.linspace(min(U),max(U),200)
    plt.xticks(xfen,fontsize=4)

    plt.ylim(min(I),max(I))
    yfen = np.linspace(min(I),max(I),200)
    plt.yticks(yfen,fontsize=5)

    # if not os.path.exists(".\\graph"):
    #     os.mkdir('.\\graph')
    plt.savefig(filename)

if __name__ == "__main__":
    filename='a.csv'
    extractFromCsv(filename)
    drawPic(filename.split(".")[0]+".png")

