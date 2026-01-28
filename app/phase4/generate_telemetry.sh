#!/bin/bash

echo "Starting telemetry generator..."
echo "Press Ctrl+C to stop"

while true; do
    # CPU: 0-100%
    echo $((RANDOM % 101)) > /tmp/cpu_telemetry.txt
    
    # RAM: 20-90%
    echo $((20 + RANDOM % 71)) > /tmp/ram_telemetry.txt
    
    # GPU: 0-100%
    echo $((RANDOM % 101)) > /tmp/gpu_telemetry.txt
    
    sleep 0.3
done