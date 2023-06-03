import sper
ui = sper.UI()
ui.createBoard(5, 5, 1)
ui.addPin(2, 2, 0)
ui.addFanout(4, 2, 0)
ui.setMode(0)
ui.getResult()
ui.outputResult()