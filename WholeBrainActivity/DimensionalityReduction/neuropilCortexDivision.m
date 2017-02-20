function  neuropilCortexDivision(CellRep,UCMovie,UNMovie,LNMovie,LCMovie)
%% Divide the 3Dcube cell array into neuropil and cortex respectively considering only the upper half of the larvae as the signal is identical in both sides of the VNC.

        %% Upper Cortex area is from the 1nd-4th row of the cell array  
        
        upCortex = cell(4,39,7); % pre-allocation of memory
        
        for UCrow = 2:5
            for UCcol = 1:39
                for UCpage = (size(CellRep,3)-6):(size(CellRep,3))%start to end of the 3rd dim (7 pages)
                    upCortex(UCrow,UCcol,UCpage) = CellRep(UCrow,UCcol,UCpage); % storing only the upper cortex part of cubes to a new variable for further qualtifications
                    upCortex{UCrow,UCcol,UCpage}(:,:,2) = []; % flatten the 3D 13x13x2 cubes to 13x13 cubes
                end
            end
        end
        
        upCortex(1,:,:)=[]; % removing the unwanted cell array
        
        for unwantedUCPage = 1:(size(CellRep,3)-7) 
            upCortex(:,:,1)=[]; % removing the unwanted pages in the matrix
        end 
        
        UCcellToMatrix = cell2mat(upCortex);figure; % converting all the flattened uppercortex cubes in the form of cells to a 2D matrix 
        UCmatrix = vertcat(UCcellToMatrix(:,:,1),UCcellToMatrix(:,:,2),UCcellToMatrix(:,:,3),UCcellToMatrix(:,:,4),UCcellToMatrix(:,:,5),UCcellToMatrix(:,:,6),UCcellToMatrix(:,:,7));
        
         ylim auto; xlim auto;
        imagesc(UCmatrix);colormap('hot'); % displaying the matrix as a image
         

        UCframe = getframe; % takes each frame generated and feds to the VideoWriter
        writeVideo(UCMovie,UCframe); % makes a video of all the frames generated
        close all;
               
        %% Upper Neuropil area is from the 6th-8th row of the cell array
        
        upNeuropil = cell(3,39,7); % pre-allocation of memory
        
        for UNrow = 6:8
            for UNcol = 1:39
                for UNpage = (size(CellRep,3)-6):(size(CellRep,3))%start to end of the 3rd dim (7 pages)
                    upNeuropil(UNrow,UNcol,UNpage) = CellRep(UNrow,UNcol,UNpage);
                    upNeuropil{UNrow,UNcol,UNpage}(:,:,2) = [];
                end
            end
        end
        
        
        for unwantedupNeuropil = 1:5 % removing the unwanted cell array
            upNeuropil(1,:,:)=[];
        end    
            
        for unwantedUNPage = 1:(size(CellRep,3)-7) % removing the unwanted empty pages in the matrix
            upNeuropil(:,:,1)=[];
        end     
           
        UNcellToMatrix = cell2mat(upNeuropil);figure;
        UNmatrix = vertcat(UNcellToMatrix(:,:,1),UNcellToMatrix(:,:,2),UNcellToMatrix(:,:,3),UNcellToMatrix(:,:,4),UNcellToMatrix(:,:,5),UNcellToMatrix(:,:,6),UNcellToMatrix(:,:,7));
        
        ylim auto; xlim auto;
        imagesc(UNmatrix);colormap('hot'); % displaying the matrix as a image

        UNframe = getframe; % takes each frame generated and feds to the VideoWriter
        writeVideo(UNMovie,UNframe); % makes a video of all the frames generated
         close all;
              
        %% Lower Neuropil area is from the 9th-11th row of the cell array
        
        loNeuropil = cell(3,39,7); % pre-allocation of memory
        
        for LNrow = 9:11
            for LNcol = 1:39
                for LNpage = (size(CellRep,3)-6):(size(CellRep,3))%start to end of the 3rd dim (7 pages)
                    loNeuropil(LNrow,LNcol,LNpage) = CellRep(LNrow,LNcol,LNpage);
                    loNeuropil{LNrow,LNcol,LNpage}(:,:,2) = [];
                end
            end
        end
        
        for unwantedloNeuropil = 1:8 % removing the unwanted cell array
            loNeuropil(1,:,:)=[];
        end 
        
        for unwantedLNPage = 1:(size(CellRep,3)-7) % removing the unwanted pages in the matrix
            loNeuropil(:,:,1)=[];
        end 
  
        LNcellToMatrix = cell2mat(loNeuropil);figure;
        LNmatrix = vertcat(LNcellToMatrix(:,:,1),LNcellToMatrix(:,:,2),LNcellToMatrix(:,:,3),LNcellToMatrix(:,:,4),LNcellToMatrix(:,:,5),LNcellToMatrix(:,:,6),LNcellToMatrix(:,:,7));
        
        ylim auto; xlim auto;
        imagesc(LNmatrix);colormap('hot'); % displaying the matrix as a image

        LNframe = getframe; % takes each frame generated and feds to the VideoWriter
        writeVideo(LNMovie,LNframe); % makes a video of all the frames generated
    close all;
        
        %% Lower Cortex area is from the 12th-15th row of the cell array
        
        loCortex = cell(4,39,7); % pre-allocation of memory
        
        for LCrow = 12:15
            for LCcol = 1:39
                for LCpage = (size(CellRep,3)-6):(size(CellRep,3))%start to end of the 3rd dim (7 pages)
                    loCortex(LCrow,LCcol,LCpage) = CellRep(LCrow,LCcol,LCpage);
                    loCortex{LCrow,LCcol,LCpage}(:,:,2) = [];
                end
            end
        end
        
        
        for unwantedloCortex = 1:11 % removing the unwanted cell array
            loCortex(1,:,:)=[];
        end 
        
        for unwantedLCPage = 1:(size(CellRep,3)-7) % removing the unwanted pages in the matrix
            loCortex(:,:,1)=[];
        end 
       
        LCcellToMatrix = cell2mat(loCortex);figure;
        LCmatrix = vertcat(LCcellToMatrix(:,:,1),LCcellToMatrix(:,:,2),LCcellToMatrix(:,:,3),LCcellToMatrix(:,:,4),LCcellToMatrix(:,:,5),LCcellToMatrix(:,:,6),LCcellToMatrix(:,:,7));
        
        ylim auto; xlim auto;
        imagesc(LCmatrix);colormap('hot'); % displaying the matrix as a image

        LCframe = getframe; % takes each frame generated and feds to the VideoWriter
        writeVideo(LCMovie,LCframe); % makes a video of all the frames generated
    close all;
end