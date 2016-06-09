site_survey() {
	iface="$1"

	echo "$(iwpriv "$iface" set SiteSurvey="1"; iwpriv "$iface" get_site_survey | tail -n '+3')"
}

parse_channel_of_specific_ssid_from_site_survey_result() {
	local target_ssid="$1"

	while read -r line; do
		local ssid=$(echo "$line" | cut -b 5-36 | sed -e 's/[[:space:]]*$//')

		[ "$target_ssid" = "$ssid" ] || continue

		echo "$line" | sed 's/^\([0-9]\+\)\ .*/\1/'

		return
	done
}

parse_encryption_of_specific_ssid_from_site_survey_result() {
	local target_ssid="$1"

	while read -r line; do
		local ssid=$(echo "$line" | cut -b 5-36 | sed -e 's/[[:space:]]*$//')

		[ "$target_ssid" = "$ssid" ] || continue

		local security=$(echo "$line" | cut -b 58-79 | sed -e 's/[[:space:]]*$//')

		local encryption

		case "$security" in
			*NONE*)
				encryption="none"
				;;
			*WPA2PSK*)
				encryption="psk2"
				;;
			*)
				encryption="psk"
				;;
		esac

		local cipher

		case "$security" in
			*NONE*)
				cipher=""
				;;
			*TKIPAES*)
				cipher="tkip+aes"
				;;
			*AES*)
				cipher="aes"
				;;
			*TKIP*)
				cipher="tkip"
				;;
		esac

		encryption="${encryption}+${cipher}"

		echo "$encryption"

		return
	done
}
