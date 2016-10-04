wireless_encrypt_passwd() {
	local rawpass="$1"
	local key=$(fw_printenv ethaddr | sed 's/.*=//g' | tr '[:upper:]' '[:lower:]')
	local encpass=$(echo -n "$rawpass" | openssl aes-256-cbc -k "$key" | base64)
	echo "$encpass"
}

wireless_decrypt_passwd() {
	local encpass="$1"
	local key=$(fw_printenv ethaddr | sed 's/.*=//g' | tr '[:upper:]' '[:lower:]')
	local rawpass=$(echo -n "$encpass" | base64 -d | openssl aes-256-cbc -d -k "$key")
	echo "$rawpass"
}
