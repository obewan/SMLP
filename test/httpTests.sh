#!/bin/sh

# This is a sh script to test the http service with a real http client
# curl is required

cd ../build

./smlp  -e "myMushroomMLP.json" -i "myMushroomMLP.json" -V -H -P 8080 &

# Get the process ID of the most recently executed background command
jobId=$!

# Wait for the server to be ready
sleep 2

# Continue with some curl commands...
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/testonly)
#response=$(curl --http1.1 -v -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/testonly 2>&1)
echo "Response: $response"

# Kill the background job
kill $jobId


