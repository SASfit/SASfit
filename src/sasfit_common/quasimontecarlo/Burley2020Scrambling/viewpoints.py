#!/usr/bin/env python

import sys
import ctypes
import numpy as np
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

sequences = '''
   random
   sobol
   sobol_rds
   sobol_owen
   sobol_owen_decorrelated
   laine_karras
   faure05
   faure05_owen
'''.split()

genpoints = np.ctypeslib.load_library('genpoints', '.')
array_1d_float32 = np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='CONTIGUOUS')
# extern "C" void genpoints(const char* seq, uint32_t n, uint32_t dim, uint32_t seed, float* x);
genpoints.genpoints.result = None
genpoints.genpoints.argtypes = [ctypes.c_char_p, ctypes.c_int32, ctypes.c_int32, ctypes.c_int32, array_1d_float32]
def zeros(nvals):
    return np.zeros(nvals, dtype=np.float32)

def genPoints(npoints, udim, vdim, seed, sequence):
    upoints = zeros(npoints)
    vpoints = zeros(npoints)
    if sequence == 'sobol_owen_decorrelated':
        sequence = 'sobol_owen'
        vseed = seed + 1
    else:
        vseed = seed
    genpoints.genpoints(sequence, npoints, udim, seed, upoints)
    genpoints.genpoints(sequence, npoints, vdim, vseed, vpoints)
    return zip(upoints, vpoints)

class Colors:
    points = QColor('black')
    bg = QColor('white')
    outline = QColor('gray')
    grid16 = QColor('lightGray')
    grid4 = QColor('gray')

class PointView(QFrame):
    def __init__(self, parent=None):
        QFrame.__init__(self, parent)
        self.setFixedSize(512, 512)
        self.setAutoFillBackground(True)
        palette = QPalette(self.palette())
        palette.setColor(QPalette.Window, Colors.bg)
        self.setPalette(palette)
        self.points = []
        self.showFFT = False

    def paintEvent(self, e):
        p = QPainter(self)
        self.paint(p)

    def paint(self, p):
        if self.showFFT:
            ft = np.zeros( ( 512, 512 ) )
            for x,y in self.points:
                if x >= 0 and x < 1 and y >= 0 and y < 1:
                    ft[int(x*512),int(y*512)] = 1
            ft -= ft.mean()
            ft = abs(np.fft.fftshift(np.fft.fft2(ft)))
            ft *= 64 / ft.mean()
            ft = ft.clip(0, 255).astype(np.uint32)
            ft = (255 << 24 | ft[:,:] << 16 | ft[:,:] << 8 | ft[:,:]).flatten()
            im = QImage(ft, 512, 512, QImage.Format_RGB32)
            p.drawImage(QPoint(0, 0), im)
        else:
            s = 450
            s0, s1 = 256-s/2,256+s/2
            p.setPen(Colors.outline)
            p.drawRect(s0, s0, s, s)
            s0 = (512-s)/2
            s1 = s0 + s

            if self.sequence.startswith('faure05'):
                for u in [i/25.0 for i in range(1,25)]:
                    if u in (.2, .4, .6, .8):
                        p.setPen(Colors.grid4)
                    else:
                        p.setPen(Colors.grid16)
                    su = s0 + u*s
                    p.drawLine(su, s0, su, s1)
                    p.drawLine(s0, su, s1, su)
            else:
                for u in [i/16.0 for i in range(1,16)]:
                    if u in (.25, .5, .75):
                        p.setPen(Colors.grid4)
                    else:
                        p.setPen(Colors.grid16)
                    su = s0 + u*s
                    p.drawLine(su, s0, su, s1)
                    p.drawLine(s0, su, s1, su)

            p.setPen(QPen(Colors.points))
            for x,y in self.points:
                p.drawPoint(s0-10,(.5-y)*s+256)
                p.drawPoint((x-.5)*s+256,s0-10)
            pen = QPen(Colors.points)
            pen.setWidth(3)
            p.setPen(pen)
            for x,y in self.points:
                p.drawPoint((x-.5)*s+256,(1-y-.5)*s+256)

class Slider(QWidget):
    def __init__(self, parent, label, min, max, initial):
        QWidget.__init__(self, parent)
        self.value = None
        self.label = QLabel(label, self)
        self.slider = QSlider(Qt.Horizontal, self)
        self.text = QLineEdit(self)
        self.slider.valueChanged.connect(self.setValue)
        self.text.textChanged.connect(self.setValue)
        self.slider.setMinimum(min)
        self.slider.setMaximum(max)
        self.setValue(initial)
        l = QHBoxLayout(self)
        l.addWidget(self.label)
        l.addWidget(self.slider)
        l.addWidget(self.text)
        self.label.setFixedWidth(80)
        self.text.setFixedWidth(80)

    def setValue(self, val):
        try: val = int(str(val))
        except: return
        if self.value == val: return
        self.value = val
        self.slider.setValue(val)
        self.text.setText(str(val))

    def setMaximum(self, max):
        self.slider.setMaximum(max)

class Sampler(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.pv = PointView(self)
        self.sequencetype_label = QLabel("sequence", self)
        self.sequencetype_label.setFixedWidth(100)
        self.sequencetype_combobox = QComboBox(self)
        self.sequencetype_combobox.insertItems(0, sequences)

        self.n_slider = Slider(self, "nsamples", 1, 4096, 64)
        self.udim_slider = Slider(self, "u dim", 0, 4, 0)
        self.vdim_slider = Slider(self, "v dim", 0, 4, 1)
        self.seed_slider = Slider(self, "seed", 1, 100, 1)

        self.showFFT_checkbox = QCheckBox("show FFT", self)

        l = QVBoxLayout(self)
        l.addWidget(self.pv)

        g = QGridLayout()
        g.addWidget(self.sequencetype_label, 0, 0)
        g.addWidget(self.sequencetype_combobox, 0, 1)
        l.addLayout(g)

        l.addWidget(self.n_slider)
        l.addWidget(self.udim_slider)
        l.addWidget(self.vdim_slider)
        l.addWidget(self.seed_slider)

        h = QHBoxLayout()
        l.addLayout(h)
        h.addWidget(self.showFFT_checkbox);

        self.sequencetype_combobox.activated.connect(self.updatePoints)
        self.n_slider.slider.valueChanged.connect(self.updatePoints)
        self.udim_slider.slider.valueChanged.connect(self.updatePoints)
        self.vdim_slider.slider.valueChanged.connect(self.updatePoints)
        self.seed_slider.slider.valueChanged.connect(self.updatePoints)
        self.showFFT_checkbox.stateChanged.connect(self.updatePoints)
        self.updatePoints()

    def updatePoints(self):
        npoints = self.n_slider.value
        udim = self.udim_slider.value
        vdim = self.vdim_slider.value
        seed = self.seed_slider.value
        sequence = self.sequencetype_combobox.currentText()
        self.pv.sequence = sequence
        self.pv.udim = udim
        self.pv.vdim = vdim
        self.pv.showFFT = self.showFFT_checkbox.checkState() == Qt.Checked
        self.pv.points = genPoints(npoints, udim, vdim, seed, sequence)
        self.pv.update()

app = QApplication(sys.argv)
w = Sampler()
w.show()
app.exec_()
