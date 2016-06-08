#!/bin/sh

. /lib/netifd/netifd-wireless.sh

init_wireless_driver "$@"
maclist=""
force_channel=""

drv_rt2860v2_init_device_config() {
	config_add_string country variant log_level short_preamble
	config_add_boolean noscan
	config_add_int channel
}

drv_rt2860v2_init_iface_config() {
	config_add_string 'auth_server:host'
	config_add_string auth_secret
	config_add_int 'auth_port:port'

	config_add_string ifname apname mode ssid encryption key key1 key2 key3 key4 wps_pushbutton macfilter led
        config_add_boolean hidden isolate
        config_add_array 'maclist:list(macaddr)'
}

drv_rt2860v2_cleanup() {
	logger drv_rt2860v2_cleanup
}

rt2860v2_setup_ap() {
	local name="$1"
	local eap=0

	json_select config
	json_get_vars mode ifname ssid encryption key key1 key2 key3 key4 wps_pushbutton server port hidden isolate macfilter
	json_get_values maclist maclist

	ifconfig $ifname up

	[ -z "$force_channel" ] || iwpriv $ifname set Channel=$force_channel
	[ "$isolate" = 1 ] || isolate=0
	iwpriv $ifname set NoForwarding=$isolate

	[ "$hidden" = 1 ] || hidden=0
	iwpriv $ifname set HideSSID=$hidden

	case "$encryption" in
	psk*|mixed*)
		local enc="WPA2PSK"
		case "$encryption" in
			psk | psk+*) enc=WPAPSK;;
			psk2 | psk2*) enc=WPA2PSK;;
			mixed*) enc=WPAPSKWPA2PSK;;
		esac
		local crypto="AES"
		case "$encryption" in
			*tkip+aes*) crypto=TKIPAES;;
			*tkip*) crypto=TKIP;;
		esac
		iwpriv $ifname set AuthMode=$enc
		iwpriv $ifname set EncrypType=$crypto
		iwpriv $ifname set IEEE8021X=0
		[ "$eap" = "1" ] || iwpriv $ifname set "WPAPSK=${key}"
		iwpriv $ifname set DefaultKeyID=2
		;;
	none)
		iwpriv $ifname set AuthMode=OPEN
		iwpriv $ifname set EncrypType=NONE
		;;
	esac

	[ "$hidden" = 1 ] || iwpriv $ifname set "SSID=${ssid}"

	iwpriv $ifname set WscConfMode=0

	[ -n "$maclist" ] && {
		for m in ${maclist}; do
			logger iwpriv $ifname set ACLAddEntry="$m"
			iwpriv $ifname set ACLAddEntry="$m"
		done
	}
	case "$macfilter" in
	allow)
		iwpriv $ifname set AccessPolicy=1
		;;
	deny)
		iwpriv $ifname set AccessPolicy=2
		;;
	*)
		iwpriv $ifname set AccessPolicy=0
		;;
	esac
	json_select ..

	wireless_add_vif "$name" "$ifname"
}

rt2860v2_setup_sta() {
	local name="$1"

	json_select config
	json_get_vars mode apname ifname ssid encryption key key1 key2 key3 key4 wps_pushbutton disabled led

	[ "$disabled" = "1" ] && {
		pkill -x "/sbin/apclid"
		ifconfig $ifname down
		iwpriv $ifname set ApCliEnable=0
		return
	}

	ifconfig $ifname up

	/sbin/apclid "$ifname" &
	pid=$?
	wireless_add_process "$?" /sbin/apclid "$ifname"

	json_select ..

	wireless_add_vif "$name" "$ifname"
}

drv_rt2860v2_setup() {
	local ifname="$1"
	EXTCHA=0

	drv_rt2860v2_teardown

	json_select config
	json_get_vars variant country channel htmode hidden log_level noscan:0
	json_select ..

	if [ "$auto_channel" -gt 0 ]; then
		force_channel=""
		channel=0
		auto_channel=1
	else
		force_channel=$channel
		auto_channel=0
		[ "$channel" -lt "7" ] && EXTCHA=1
	fi

	coex=1
	[ "$noscan" -gt 0 ] && coex=0

	ssid=$(uci get wireless.ap.ssid)

	cat /etc/Wireless/${variant}_tpl.dat > /tmp/${variant}.dat
	cat >> /tmp/${variant}.dat << EOF
HT_EXTCHA=${EXTCHA:-0}
CountryCode=${country:-US}
Channel=${channel:-8}
AutoChannelSelect=${auto_channel:-0}
HideSSID=${hidden:-0}
SSID1=${ssid:-OpenWrt}
HT_BSSCoexistence=${coex:-1}
EOF
	for_each_interface "ap" rt2860v2_setup_ap
	for_each_interface "sta" rt2860v2_setup_sta
	wireless_set_up
}

rt2860v2_teardown() {
	json_select config
	json_get_vars ifname
	json_select ..

	ifconfig $ifname down
}

drv_rt2860v2_teardown() {
	wireless_process_kill_all
	for_each_interface "ap sta" rt2860v2_teardown
}

add_driver rt2860v2
