% Main of the program 
% author: Varshini Guddanti
% created : 05/15/2016


clear all



%% Video for UpperCortex Frames
UCMovie  = VideoWriter('UpperCortex.avi');
UCMovie.FrameRate = 5;
UCMovie.Quality  = 100;
open(UCMovie);

%% Video for UpperNeuropil Frames
UNMovie  = VideoWriter('UpperNeuropil.avi');
UNMovie.FrameRate = 5;
UNMovie.Quality  = 100;
open(UNMovie);

%% Video for LowerNeropil Frames
LNMovie  = VideoWriter('LowerNeropil.avi');
LNMovie.FrameRate = 5;
LNMovie.Quality  = 100;
open(LNMovie);

%% Video for LowerCortex Frames
LCMovie  = VideoWriter('LowerCortex.avi');
LCMovie.FrameRate = 5;
LCMovie.Quality  = 100;
open(LCMovie);

%% Function call to inputFor3DFigure()
inputFor3DFigure(14,UCMovie, UNMovie, LNMovie, LCMovie);

%% Closing all videoWriters
close(UCMovie);
close(UNMovie);
close(LNMovie);
close(LCMovie);

