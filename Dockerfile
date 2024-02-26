# Use an official Debian runtime as a parent image
FROM debian:latest

# Define environment variables for the model and port
ENV MODEL_NAME test/data/testModel.json
ENV PORT 8080

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy the sources directory contents into the container at /usr/src/app
COPY . .

# Install any needed packages
RUN apt-get update \
    && apt-get install -y cmake make gcc-11 g++-11 \
    && apt-get clean

# Make port available to the world outside this container
EXPOSE ${PORT}

# Configure CMake for Release
RUN mkdir build && \
    CC=gcc-11 CXX=g++-11 cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project.
RUN cmake --build build --config Release -- -j 2

# Change the working directory to /usr/src/app/build
WORKDIR /usr/src/app/build

# Run smlp when the container launches 
ENV CMD "./smlp -H -P ${PORT} -i ${MODEL_NAME}"
CMD [ "sh", "-c", "${CMD}" ]