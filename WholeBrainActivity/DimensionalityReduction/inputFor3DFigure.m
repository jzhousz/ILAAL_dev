function inputFor3DFigure(nslices,UCMovie,UNMovie,LNMovie,LCMovie)

set(0,'DefaultFigureVisible','off');
    %% takes all frames of the tif file and gives each time slice to the make3DFigure function to generate a 3D matrix and plot it
    FileTif = 'F:\varshini_neuronAnalysis\2016\01_05\01_05.tif';
    InfoImage=imfinfo(FileTif);
    nFrames=length(InfoImage);
    count = 0;
     for i = 1:nFrames
         if(mod(i,nslices) == 0)% nslices are the no.of frames to be fed to the 
              count = count+1;
              fprintf('Figure %d \n',count);
              make3DFigure(i-13,i,UCMovie,UNMovie,LNMovie,LCMovie);
         end
             
     end
end      
