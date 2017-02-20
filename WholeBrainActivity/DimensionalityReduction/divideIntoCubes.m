function M=divideIntoCubes(ThreeDimFigure,UCMovie,UNMovie,LNMovie,LCMovie)


%% Divides each 3D matrix into cubes of 13x13x2 each to do further processing
 CellRep = mat2cell(ThreeDimFigure,[13*ones(1, floor(size(ThreeDimFigure,1)/13)), mod(size(ThreeDimFigure,1), 13)],[13*ones(1, floor(size(ThreeDimFigure,2)/13)), mod(size(ThreeDimFigure,2), 13)], [2*ones(1, floor(size(ThreeDimFigure,3)/2)), mod(size(ThreeDimFigure,3), 2)]);
%  figure('Name','Test #2')
%  handles = cellplot(CellRep);
%  daspect([1 1 1])
%  view(3); axis equal tight
%  hgsave(gcf,'testCube');
 


%% delete incomplete cells from 3D cell array (16th row,40th col,last cellarray)
CellRep(16,:,:) = [];
CellRep(:,40,:) = [];
CellRep(:,:,(size(ThreeDimFigure,3)/2)+1) = [];


%% average of all pixels in each 13x13x2 cube
     for row = 1:15
         for col = 1:39
            for page = ((size(ThreeDimFigure,3)/2)-6):(size(ThreeDimFigure,3)/2)%start to end of the 3rd dim (7 pages)
                CellRep{row,col,page}(:) = mean(CellRep{row,col,page}(:));
            end
         end
     end
% celldisp(CellRep);

%% Divide half of the upper half of the larva into neuropil and cortex
  neuropilCortexDivision(CellRep,UCMovie,UNMovie,LNMovie,LCMovie);
   

end


                                                                                          