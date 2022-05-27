// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v4 C library
//
//  build: go build -buildmode c-shared -o libteonet.so .
//
package main

// #include <stdio.h>
// #include <stdlib.h>
//
// typedef unsigned char (*c_reader)(int teo, char *addr, void* data, int dataLen, unsigned char ev);
// unsigned char runReaderCb(c_reader cb, int teo, char *addr, void* data, int dataLen, unsigned char ev);
//
// typedef unsigned char (*c_api_reader)(int teoApi, void *data, int dataLen, char *err);
// unsigned char runAPIReaderCb(c_api_reader c_reader, int teoApi, void *data, int dataLen, char *err);
//
// void safe_printf();
//
import "C"
import (
	"unsafe"

	"github.com/teonet-go/teonet"
)

const (
	version = "0.2.1"
)

func main() {}

// teoEvData is Teonet Event Data constant
//export teoEvData
func teoEvData() C.uchar {
	return C.uchar(teonet.EventData)
}

// teoLogo print teonet logo
//export teoLogo
func teoLogo(c_appName *C.char, c_appVersion *C.char) {
	appName := C.GoString(c_appName)
	appVersion := C.GoString(c_appVersion)
	teonet.Logo(appName, appVersion)
}

// teoVersion return string with Teonet version, should be freed after using
//export teoVersion
func teoVersion() (c_address *C.char) {
	version := teonet.Version
	return C.CString(version)
}

// teoCVersion return string with Teonet-c (this library) version, should be
// freed after using
//export teoCVersion
func teoCVersion() (c_address *C.char) {
	return C.CString(version)
}

// teoNew start teonet client, return digital key to use Teonet pointer, or zero
// at error
//export teoNew
func teoNew(c_appShort *C.char) (teoKey C.int) {
	appShort := C.GoString(c_appShort)
	teo, err := teonet.New(appShort)
	/* params.appShort, params.port, reader, teonet.Log(), "NONE",
	params.showTrudp, params.logLevel, teonet.LogFilterT(params.logFilter */
	if err != nil {
		teo.Log().Debug.Println("can't init Teonet, error:", err)
		return
	}
	return teoc.add(teo)
}

// teoConnect connect to teonet, return true if ok
//export teoConnect
func teoConnect(c_teo C.int) (ok C.uchar) {
	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	err := teo.Connect()
	if err == nil {
		ok = 1
	}
	return
}

// teoAddress return string with Teonet address, should be freed after using
//export teoAddress
func teoAddress(c_teo C.int) (c_address *C.char) {
	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	address := teo.Address()
	return C.CString(address)
}

// teoConnectTo connect to teonet peer, return true if ok
//export teoConnectTo
func teoConnectTo(c_teo C.int, c_address *C.char) (ok C.uchar) {
	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	address := C.GoString(c_address)
	err := teo.ConnectTo(address)
	if err == nil {
		ok = 1
	}
	return
}

// teoConnectToCb connect to teonet peer with reader callback, return true if ok
// reader will receive data from peer
//export teoConnectToCb
func teoConnectToCb(c_teo C.int, c_address *C.char, c_reader unsafe.Pointer) (ok C.uchar) {
	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	address := C.GoString(c_address)
	err := teo.ConnectTo(address, func(c *teonet.Channel, p *teonet.Packet,
		e *teonet.Event) (processed bool) {

		if e.Event != teonet.EventData {
			return
		}

		data := p.Data()
		dataPtr := unsafe.Pointer(nil)
		if data != nil {
			dataPtr = unsafe.Pointer(&data[0])
		}
		addr := C.CString(c.Address())

		if C.runReaderCb(C.c_reader(c_reader),
			c_teo,
			addr,
			dataPtr,
			C.int(len(data)),
			C.uchar(e.Event),
		) != 0 {
			processed = true
		}
		C.free(unsafe.Pointer(addr))
		return
	})
	if err == nil {
		ok = 1
	}
	return
}

// teoSendTo send data to teonet peer, return true if ok
//export teoSendTo
func teoSendTo(c_teo C.int, c_address *C.char, c_data unsafe.Pointer,
	c_data_len C.int) (ok C.uchar) {

	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	address := C.GoString(c_address)
	data := C.GoBytes(c_data, c_data_len)
	_, err := teo.SendTo(address, data)
	if err == nil {
		ok = 1
	}

	return
}

// teoSendCmdTo send command with data to teonet peer, return true if ok
//export teoSendCmdTo
func teoSendCmdTo(c_teo C.int, c_address *C.char, c_cmd C.uchar,
	c_data unsafe.Pointer, c_data_len C.int) (ok C.uchar) {

	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	address := C.GoString(c_address)
	data := C.GoBytes(c_data, c_data_len)
	cmd := byte(c_cmd)
	_, err := teo.Command(cmd, data).SendTo(address)
	if err == nil {
		ok = 1
	}

	return
}

// teoWaitForever wait forever
//export teoWaitForever
func teoWaitForever(c_teo C.int) {
	select {}
}

// teoApiClientNew create and return pointer to new API Client interface, it
// return nil at error
//export teoApiClientNew
func teoApiClientNew(c_teo C.int, c_address *C.char) (teoApiKey C.int) {
	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	address := C.GoString(c_address)
	apicli, err := teo.NewAPIClient(address)
	if err != nil {
		return
	}

	return teoc.add(apicli)
}

// teoApiSendCmdTo send api command with data to teonet peer, return true if ok
//export teoApiSendCmdTo
func teoApiSendCmdTo(c_teoApi C.int, c_cmd C.uchar,
	c_data unsafe.Pointer, c_data_len C.int) (ok C.uchar) {

	apicli, ok := teoc.getTeoApi(c_teoApi)
	if ok == 0 {
		return
	}

	data := C.GoBytes(c_data, c_data_len)
	cmd := byte(c_cmd)
	_, err := apicli.SendTo(cmd, data)
	if err == nil {
		ok = 1
	}

	return
}

// teoApiSendCmdToCb send api command with data to teonet peer, return true if ok
//export teoApiSendCmdToCb
func teoApiSendCmdToCb(c_teoApi C.int, c_cmd C.uchar,
	c_data unsafe.Pointer, c_data_len C.int, c_reader unsafe.Pointer) (ok C.uchar) {

	apicli, ok := teoc.getTeoApi(c_teoApi)
	if ok == 0 {
		return
	}

	data := C.GoBytes(c_data, c_data_len)
	cmd := byte(c_cmd)
	_, err := apicli.SendTo(cmd, data, func(data []byte, err error) {

		// Get error
		var c_err *C.char
		if err != nil {
			c_err = C.CString(err.Error())
		}

		// Get data pointer
		var dataPtr = unsafe.Pointer(nil)
		if data != nil {
			dataPtr = unsafe.Pointer(&data[0])
		}

		// Execute API reader callback
		C.runAPIReaderCb(C.c_api_reader(c_reader),
			c_teoApi,
			dataPtr,
			C.int(len(data)),
			c_err,
		)
	})
	if err == nil {
		ok = 1
	}

	return
}

// teoApiAddress return string with Teonet API Client address, should be freed
// after using
//export teoApiAddress
func teoApiAddress(c_teoApi C.int) (c_address *C.char) {

	apicli, ok := teoc.getTeoApi(c_teoApi)
	if ok == 0 {
		return
	}

	address := apicli.Address()
	return C.CString(address)
}
