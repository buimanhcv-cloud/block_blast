#!/bin/bash
# ── Chạy script này trên Oracle VM lần đầu tiên ──────────────────────────────
# ssh vào VM rồi chạy: bash setup_oracle_server.sh

echo "=== Installing Node.js ==="
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt-get install -y nodejs

echo "=== Installing PM2 ==="
sudo npm install -g pm2

echo "=== Opening port 5000 in Ubuntu firewall ==="
sudo iptables -I INPUT -p tcp --dport 5000 -j ACCEPT
sudo apt-get install -y iptables-persistent
sudo netfilter-persistent save

echo "=== Starting Block Puzzle Server ==="
pm2 start server.js --name "block-puzzle"
pm2 startup
pm2 save

echo ""
echo "=== DONE ==="
echo "Server is running on port 5000"
echo "Check status: pm2 status"
echo "View logs:    pm2 logs block-puzzle"
