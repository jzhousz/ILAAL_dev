function make3DFigure( FirstSlice, LastSlice,UCMovie,UNMovie,LNMovie,LCMovie )


%% Convert first 14 frames of each time frame into a 3D matrix 

    FileTif = 'F:\varshini_neuronAnalysis\2016\01_05\01_05.tif';
    MatrixName = zeros(200,512,14); % pre-allocation of memory
    
    for m = FirstSlice:LastSlice
        MatrixName(:,:,m) = imread(FileTif,'Index',m); % builds a 3D matrix/figure with every 14 frames of the video
    end
    
    
%      figure('Name','Test #1')
%      patch(isocaps(MatrixName,1.5),'FaceColor','interp','EdgeColor','none');
%      daspect([1 1 1])
%      view(3); axis equal tight
%      hgsave(gcf,'testFig1');
     
     divideIntoCubes(MatrixName,UCMovie,UNMovie,LNMovie,LCMovie);% pass the matrix to get cubes
     
end
