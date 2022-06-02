import xlwt
import xlrd

data = xlrd.open_workbook("fct_analysis_file.xls")
table = data.sheet_by_name("fct_data")
rowNum = table.nrows

ofile = xlwt.Workbook(encoding='utf-8', style_compression=0)
sheet0 = ofile.add_sheet("0")
sheet1 = ofile.add_sheet("1")
sheet2 = ofile.add_sheet("2")

row0 = 0
row1 = 0
row2 = 0

for i in range(rowNum):
    if(int(table.row(i)[0].value)==0):
        sheet0.write(row0,0,0)
        sheet0.write(row0,1,table.row(i)[1].value)
        sheet0.write(row0,2,table.row(i)[2].value)
        row0 = row0+1
    elif(int(table.row(i)[0].value)==1):
        sheet1.write(row1,0,1)
        sheet1.write(row1,1,table.row(i)[1].value)
        sheet1.write(row1,2,table.row(i)[2].value)
        row1 = row1+1
    else:
        sheet2.write(row2,0,2)
        sheet2.write(row2,1,table.row(i)[1].value)
        sheet2.write(row2,2,table.row(i)[2].value)
        row2 = row2+1

ofile.save("data_classify.xls")

