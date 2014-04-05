
# analyze color sensor data

import numpy as np
import matplotlib.pyplot as plt
import csv

RED_FILENAME = 'data/redTest.txt'
GREEN_FILENAME = 'data/greenTest.txt'
BLUE_FILENAME = 'data/blueTest.txt'
WHITE_FILENAME = 'data/whiteTest.txt'

with open(RED_FILENAME, 'rb') as csvfile:
    reader = csv.reader(csvfile, delimiter='\t')
    for row in reader:
        print ', '.join(row)