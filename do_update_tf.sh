export TF_BINARY_URL=https://storage.googleapis.com/tensorflow/linux/gpu/tensorflow-0.11.0rc2-cp27-none-linux_x86_64.whl

sudo pip install --upgrade $TF_BINARY_URL

python -c 'import tensorflow as tf; print(tf.__version__)'  

