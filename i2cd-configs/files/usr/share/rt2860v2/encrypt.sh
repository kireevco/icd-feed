get_encrypted_ra_key() {
	local realkey="$1"
	local apaddr=$(uci get wireless.ap.macaddr)
	local keykey=$(echo -n "$apaddr" | awk -F ":" '{print $4""$5""$6 }' | tr [:lower:] [:upper:])
	local key=$(echo -n "$realkey" | openssl aes-256-cbc -k "$keykey" | base64)
	echo "$key"
}

get_decrypted_ra_key() {
	local key="$1"
	local apaddr=$(uci get wireless.ap.macaddr)
	local keykey=$(echo -n "$apaddr" | awk -F ":" '{print $4""$5""$6 }' | tr [:lower:] [:upper:])
	local realkey=$(echo -n "$key" | base64 -d | openssl aes-256-cbc -d -k "$keykey")
	echo "$realkey"
}

get_encrypted_sta_key() {
	local realkey="$1"
	local staaddr=$(uci get wireless.sta.macaddr)
	local keykey=$(echo -n "$staaddr" | awk -F ":" '{print $4""$5""$6 }' | tr [:lower:] [:upper:])
	local key=$(echo -n "$realkey" | openssl aes-256-cbc -k "$keykey" | base64)
	echo "$key"
}

get_decrypted_sta_key() {
	local key="$1"
	local staaddr=$(uci get wireless.sta.macaddr)
	local keykey=$(echo -n "$staaddr" | awk -F ":" '{print $4""$5""$6 }' | tr [:lower:] [:upper:])
	local realkey=$(echo -n "$key" | base64 -d | openssl aes-256-cbc -d -k "$keykey")
	echo "$realkey"
}
