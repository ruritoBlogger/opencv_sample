#!/bin/bash

echo start
sudo apt update && sudo apt upgrade

# install tensorflow
sudo apt-get install libhdf5-serial-dev hdf5-tools libhdf5-dev zlib1g-dev zip libjpeg8-dev liblapack-dev libblas-dev gfortran
sudo apt-get install python3-pip
sudo pip3 install -U pip testresources setuptools=49.6.0 
sudo pip3 install -U numpy==1.16.1 future==0.18.2 mock==3.0.5 h5py==2.10.0 keras_preprocessing==1.1.1 keras_applications==1.0.8 gast==0.2.2 futures protobuf pybind11

sudo pip3 install --pre --extra-index-url https://developer.download.nvidia.com/compute/redist/jp/v44 tensorflow

# install python packages
sudo pip3 install -U scikit-learn, Pillow, joblib, pyzmq, cupy, scikit-image, sqlalchemy

# install node.js
sudo apt install -y nodejs npm
sudo npm install n -g
sudo n stable

sudo apt purge -y nodejs npm
exec $SHELL -l

# install yarn
sudo npm install -g yarn

echo end
