#!/bin/sh

# This is a sh script to test the http service with a real http client
# requirements: curl, jq

cd ../build

./smlp  -e "ctestModel.json" -i "ctestModel.json" -V -H -P 8080 &

# Get the process ID of the most recently executed background command
jobId=$!

# Wait for the server to be ready
sleep 2

# Continue with some curl commands...
#response=$(curl --http1.1 -v -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/testonly 2>&1)

echo "[CLIENT] **** TESTS START ****"

echo "\n[CLIENT] TEST TRAINONLY"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/trainonly)
echo "[CLIENT] trainonly response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 200 -a $code -ne 0 ]; then    
    echo "Test failed: code is not 200 or 0"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

echo "\n[CLIENT] TEST TESTONLY"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/testonly)
echo "[CLIENT] testonly response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 200 -a $code -ne 0 ]; then    
    echo "Test failed: code is not 200 or 0"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

echo "\n[CLIENT] TEST TRAINTHENTEST"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/trainthentest)
echo "[CLIENT] trainthentest response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 200 -a $code -ne 0 ]; then    
    echo "Test failed: code is not 200 or 0"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

echo "\n[CLIENT] TEST TRAINTESTMONITORED 1"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/traintestmonitored)
echo "[CLIENT] traintestmonitored 1 response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 200 -a $code -ne 0 ]; then    
    echo "Test failed: code is not 200 or 0"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

# Wait for the server to do a save (part of the test)
sleep 2

echo "\n[CLIENT] TEST TRAINTESTMONITORED 2"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/traintestmonitored)
echo "[CLIENT] traintestmonitored 2 response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 200 -a $code -ne 0 ]; then    
    echo "Test failed: code is not 200 or 0"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

echo "\n[CLIENT] TEST PREDICT"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/predict)
echo "[CLIENT] predict response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 200 -a $code -ne 0 ]; then    
    echo "Test failed: code is not 200 or 0"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

# Testing error cases
echo "\n[CLIENT] TEST BAD PATH"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/oops)
echo "[CLIENT] test bad path response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 400 ]; then    
    echo "Test failed: code is not 400"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

echo "\n[CLIENT] TEST BAD METHOD"
response=$(curl -s -X GET -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/testonly)
echo "[CLIENT] test bad method response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 400 ]; then    
    echo "Test failed: code is not 400"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

echo "\n[CLIENT] TEST BAD CONTENT 1"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '' http://localhost:8080/testonly)
echo "[CLIENT] test bad content 1 response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 400 ]; then    
    echo "Test failed: code is not 400"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

echo "\n[CLIENT] TEST BAD CONTENT 2"
response=$(curl -s -X POST -H "Content-Type: text/plain" -d 'oops' http://localhost:8080/testonly)
echo "[CLIENT] test bad content 2 response: $response"
code=$(echo $response | jq -r '.code')
if [ $code -ne 400 ]; then    
    echo "Test failed: code is not 400"
    kill $jobId
    echo "\n[CLIENT] **** TESTS END : FAILURE ****"
    exit
fi

# Kill the background job
kill $jobId

echo "\n[CLIENT] **** TESTS END : SUCCESS ****"

#rq: if the job is not killed, try a kill -9 <PID>, with <PID> the PID of the process (command "ps ux | grep smlp")


