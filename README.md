
# Poly Render
This is a collection of various scripts, which can help you create nice images from poly files. Poly files are the output of pomelo, which you can find at https://github.com/spatialfruitsalad/pomelo  or http://theorie1.physik.uni-erlangen.de/research/pomelo/index.html .

![test](https://cloud.githubusercontent.com/assets/17979038/25128693/dea86b48-243a-11e7-8a95-6b9cc504605d.png)

## Example Video File
Download the video file here: https://faubox.rrze.uni-erlangen.de/dl/fi7Ukc1QAq97YY6BugCmQd7s/w020_stereo5_jpeg.avi

Watch the video on youtube: https://youtu.be/D3qs4lO7-jY

To watch it on your android smartphone, get a google cardboard (~20$) here: https://vr.google.com/cardboard/


## TL;DR
There is a single script that creates stereo images from an `ellip` (particle coordinates) file. 
The script is called `creatorFromEllip.bsh` and expects one parameter: the absolute path to the ellip file. 
The results are a bunch of stereo image files, that can be loaded into imagej to create a stereo video from them. 
See the section create a video from multiple images on how to do that.

Note that depending on your file and folder structure, some variables on the script have to be changed. They are marked within the skript. You need the programs pomelo, karambola and povray installed and make them known to the script.
The lines that need to be changed are the following
```
POMELO=/home/sweis/setvoronoi/bin/pomelo
KARAMBOLA=/home/sweis/karambola-NJP_documented/karambola
THISSCRIPTFOLDER=/home/sweis/projects/polyRender/
POVRAY=povray

## Half the offset of the "virtual eyes" in units of voxels. My particles have a diameter of around 50 voxels and 2* 5 voxels offset between left and right eye works fine
OFFSET=5

```

# How to create 3D videos -- detailed description
## pomelo
Pomelo is a gereric Set Voronoi Program that can be obtained from http://theorie1.physik.uni-erlangen.de/research/pomelo/index.html .
Pomelo calculates the Voronoi Diagram of the sphere packing using generic mode based on an `*.ellip` file. 
A complete example with read, config and ellip file is given in the folder `test/2015-12-22_spheres_center/` of the pomelo folder.

The input of pomelo is the `ellip` file, the output created is a `cell.poly` file. 
Note, that an `ellip` file describes ellipsoids as particles(center x,y,z, axis length, axis orientation (x,y,z) for axis 1,2,3), but the lua file assumes spheres. All steps described here are meant for sphere packings.

The command looks like this:

```
./pomelo -mode GENERIC -i ../test/2015-12-22_spheres_center/spheresCenter.lua -o spheresCenter -POLY
```

The first parameter is the mode, which is here set to `GENERIC` (build pomelo with `make GENERIC` to enable this mode). The parameters that follow are -i and -o for input and output respectively. Please see the pomelo documentation for further details.

## cutting the poly file 
There are a lot of Voronoi cells in the `poly` file (approximately 3000). It is sufficient to just watch about 50 of them at the center of the packing.
Therefore the program `polyCut` has been developed. This can cut out certain Voronoi cells from one large `poly` file by their labels.
To get the respective label IDs from the center of the packing, use the program getLabels.bsh on the ellip file

```
polycut/getLabels.bsh ellipsoids_linear_std5.ellip
```

The result can be used for the cutpoly file
```
polycut/polyCut cell.poly cell.poly.cut [IDs from getLabels.bsh]
```

Which will result in a file cell.poly.cut


## Minkowski Tensor calculation
The Minkowski Tensors are needed for the calculation of the anisotropy of the Voronoi cells. The program karambola can be used to calculate those values
Use it like this:
```
karambola --labels --reference-centroid -i cell.poly.cut -o karambola_out
```

-i and -o specify the input and output files/folders. --labels uses the labes of the cells (which is needed to assign the cell anisotropy to the particle IDs. --reference-centroid is needed because the cells are at their respective position (without the option, karambola assumes the origin (0,0,0) as the reference, which will lead to wrong minkowski values).



## poly2pov
To convert a `poly` file (which contains a Voronoi tesselation) to a nice png image, povray is used. povray cannot read poly directly, so the poly file has to be converted. This is done with the program poly2pov.

```
./poly2pov [polyFile] [ellipFile] [w020_eigsysFile] [infoName] (angle) (xshift)
```

This program will create a pov file, that contains the information to draw a nice looking png image of the pspherical particles (therefore the ellip file is needed) and the respective voronoi cells.

## povray

povray can be called on the created polyfile and will create a png image from the pov file.

![rendering of a sphere packing](https://cloud.githubusercontent.com/assets/17979038/25128693/dea86b48-243a-11e7-8a95-6b9cc504605d.png)

## create multiple images at different rotations
Videos are just a bunch of sequential images. To create a video of a rotating object, we need to create many images of that object at various rotation angles. This can be done with the poly2pov program via the (angle) option. 
Each call will create a respective pov file which can then be rendered to a png with povray.

Note that there is a script createRotation.bsh which does exactly this. The arguments are:
```
createRotation.bsh POLYFILE CENTERFILE W020FILE XSHIFT
```

## create a video from multiple images
Videos can be rendered with imagej. Therefore open up imagej and click file -> import -> image sequence, which will load all image files in a folder. Do this for the created png files.
Then click file -> save as -> avi. The option framerate/fps determines how fast the video will play. I recommend at least 30 fps. As a compression use jpeg, as this is the normal decode option for avi videos.

### stereo videos ###
To create a stereo video, a left and a right image is requied. a Top down sketch could look like this: o are the objects, V are both eyes (/ and \ are the lines from eye position to the position where the eye is looking) and x is the place, to where both eyes look.

      o
     ooo
    ooooo
     oxo
     /o\
    /   \
   V     V

The program `poly2pov` has a parameter xshift, which describes the offset of the camera to the central axis. You need to set the same amount of xshift in positive and negative direction. The images are then merged with the program stereoImageCreator which takes two images and combines them left and right to a stereo image. These images can then be converted to an avi video with the same procedure as described above.

