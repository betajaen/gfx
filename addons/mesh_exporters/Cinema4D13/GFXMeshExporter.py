import c4d
from c4d import bitmaps, gui, storage

def main():

    path = storage.SaveDialog(type=c4d.FILESELECTTYPE_ANYTHING, title="Save to...")
    if not path: return
    
    f = open(path, 'w')
    f.write('# Exported by C4D\n')
    
    selection = doc.GetSelection()
    if selection is not None:
        for polyObject in selection:
            #isSelected = polyObject.GetPolygonS().GetAll(polyObject.GetPolygonCount())
            obj_pnts = polyObject.GetAllPoints()
            
            lineLength = 0
            newLine = True
            
            
            f.write('position=3 float\n')
            
            for i in xrange(len(obj_pnts)):
                if newLine:
                    f.write('position')
                    newLine = False;
                    lineLength = 0
                
                s = (" {0} {1} {2}".format(obj_pnts[i].x, obj_pnts[i].y, obj_pnts[i].z))
                f.write(s)
                
                lineLength += len(s)
                if lineLength > 64:
                    f.write('\n')
                    newLine = True
            
            if newLine == False:
               f.write('\n') 
            
            f.write('index=uint16\n')
            newLine = True
            lineLength = 0
            
            for i in xrange(polyObject.GetPolygonCount()):
                poly = polyObject.GetPolygon(i)
                
                if newLine:
                    f.write('index')
                    newLine = False
                    lineLength = 0
                
                s = (" {0} {1} {2}".format(poly.a, poly.b, poly.c))
                f.write(s)
                 
                lineLength += len(s)
                if lineLength > 64:
                    f.write('\n')
                    newLine = True
            
            if newLine == False:
               f.write('\n') 
            
            
    f.close()
    
if __name__=='__main__':
    main()