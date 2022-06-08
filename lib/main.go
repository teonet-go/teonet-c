// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v5 C library
//
//  build: go build -buildmode c-shared -o libteonet.so .
//
package main

// The comment block below is cgo comments. It contains includes, type
// definitions and function definitions which copy to the Teonet-c library
// include file during go build. This definitions and code used in this go code.
// Be careful when edit this comments.

//// CGO block
//
// #include "c_callbacks.h"
// #include <stdlib.h>
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
func teoNew(c_appShort *C.char) (c_teo C.int) {
	return _teoNew(c_appShort, nil)
}

// teoNewCb start teonet client with reader, return digital key to use Teonet
// pointer, or zero at error
//export teoNewCb
func teoNewCb(c_appShort *C.char, c_reader unsafe.Pointer) (c_teo C.int) {
	return _teoNew(c_appShort, c_reader)
}

// _teoNew start teonet client with or without reader, return digital key to
// use Teonet pointer, or zero at error
func _teoNew(c_appShort *C.char, c_reader unsafe.Pointer) (c_teo C.int) {
	appShort := C.GoString(c_appShort)
	var teo *teonet.Teonet
	var err error
	// Reader - receive and process incoming messages
	reader := func(c *teonet.Channel, p *teonet.Packet, e *teonet.Event) (processed bool) {

		// Prepare params
		var data []byte
		if e.Event == teonet.EventData {
			data = p.Data()
		}
		dataPtr := unsafe.Pointer(nil)
		if data != nil {
			dataPtr = unsafe.Pointer(&data[0])
		}
		addr := C.CString(c.Address())

		// Execute C callback
		if C.runReaderCb(C.c_reader(c_reader),
			c_teo,
			addr,
			dataPtr,
			C.int(len(data)),
			C.uchar(e.Event),
			nil,
		) != 0 {
			processed = true
		}
		C.free(unsafe.Pointer(addr))

		return
	}

	if c_reader != nil {
		teo, err = teonet.New(appShort, reader)
	} else {
		teo, err = teonet.New(appShort)
	}
	if err != nil {
		teo.Log().Debug.Println("can't init Teonet, error:", err)
		return
	}

	c_teo = teoc.add(teo)
	return
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

// teoConnectToCb connect to teonet peer with reader callback, return true if ok.
// The reader will receive data from peer
//export teoConnectToCb
func teoConnectToCb(c_teo C.int, c_address *C.char, c_reader unsafe.Pointer,
	c_user_data unsafe.Pointer) (ok C.uchar) {

	if c_reader == nil {
		return teoConnectTo(c_teo, c_address)
	}
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
			c_user_data,
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

// teoApicliNew create and return pointer to new API Client interface, it
// return nil at error
//export teoApicliNew
func teoApicliNew(c_teo C.int, c_address *C.char) (teoApiKey C.int) {
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

	apicli, ok := teoc.getTeoApiCli(c_teoApi)
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

// teoApicliSendCmdToCb send api client command with data to teonet peer, return
// true if ok
//export teoApicliSendCmdToCb
func teoApicliSendCmdToCb(c_teoApi C.int, c_cmd C.uchar, c_data unsafe.Pointer,
	c_data_len C.int, c_reader unsafe.Pointer,
	user_data unsafe.Pointer) (ok C.uchar) {
	return _teoApicliSendCmdToCb(c_teoApi, byte(c_cmd), c_data, c_data_len, c_reader,
		user_data)
}

// teoApicliSendCmdToStrCb send api client command with data to teonet peer,
// return true if ok
//export teoApicliSendCmdToStrCb
func teoApicliSendCmdToStrCb(c_teoApi C.int, c_cmd *C.char, c_data unsafe.Pointer,
	c_data_len C.int, c_reader unsafe.Pointer,
	user_data unsafe.Pointer) (ok C.uchar) {
	cmd := C.GoString(c_cmd)
	return _teoApicliSendCmdToCb(c_teoApi, cmd, c_data, c_data_len, c_reader,
		user_data)
}

// _teoApicliSendCmdToStrCb generic send api client command with data to teonet
// peer where
// cmd may be byte or string, return true if ok
func _teoApicliSendCmdToCb[CMD byte | string](c_teoApi C.int, cmd CMD,
	c_data unsafe.Pointer, c_data_len C.int, c_reader unsafe.Pointer,
	user_data unsafe.Pointer) (ok C.uchar) {

	apicli, ok := teoc.getTeoApiCli(c_teoApi)
	if ok == 0 {
		return
	}

	data := C.GoBytes(c_data, c_data_len)
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
			nil,
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

	apicli, ok := teoc.getTeoApiCli(c_teoApi)
	if ok == 0 {
		return
	}

	address := apicli.Address()
	return C.CString(address)
}

// teoApiNew create and return pointer to new API interface, it return nil at
// error
//export teoApiNew
func teoApiNew(c_teo C.int, c_appName, c_appShort, c_appLong, c_appVersion *C.char) (teoApiKey C.int) {
	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}
	appName := C.GoString(c_appName)
	appShort := C.GoString(c_appShort)
	appLong := C.GoString(c_appLong)
	appVersion := C.GoString(c_appVersion)
	api := teo.NewAPI(appName, appShort, appLong, appVersion)

	return teoc.add(api)
}

// cmdApiType
type cmdApiType struct {
	c_api C.int
	*teonet.APIData
}

// teoMakeAPI2 is Teonet API builder
//export teoMakeAPI2
func teoMakeAPI2(c_api C.int) (teoApiDataKey C.int) {

	cmdApi := new(cmdApiType)
	cmdApi.c_api = c_api
	cmdApi.APIData = teonet.MakeAPI2()

	return teoc.add(cmdApi)
}

// teoApiSetCmd set APIData command number
//export teoApiSetCmd
func teoApiSetCmd(c_cmdApi C.int, cmd C.int) {
	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}
	cmdApi.SetCmd(129)
}

// teoApiSetName set APIData command name
//export teoApiSetName
func teoApiSetName(c_cmdApi C.int, c_name *C.char) {
	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}
	name := C.GoString(c_name)
	cmdApi.SetName(name)
}

// teoApiSetShort set APIData command short description
//export teoApiSetShort
func teoApiSetShort(c_cmdApi C.int, c_short *C.char) {
	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}
	short := C.GoString(c_short)
	cmdApi.SetShort(short)
}

// teoApiSetUsage set APIData command usage
//export teoApiSetUsage
func teoApiSetUsage(c_cmdApi C.int, c_usage *C.char) {
	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}
	usage := C.GoString(c_usage)
	cmdApi.SetUsage(usage)
}

// teoApiSetReturn set APIData command return description
//export teoApiSetReturn
func teoApiSetReturn(c_cmdApi C.int, c_return *C.char) {
	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}
	ret := C.GoString(c_return)
	cmdApi.SetReturn(ret)
}

// API answer mode constants
//export teoApiAnswerData
func teoApiAnswerData() C.int { return C.int(teonet.DataAnswer) }

//export teoApiAnswerCmd
func teoApiAnswerCmd() C.int { return C.int(teonet.CmdAnswer) }

//export teoApiAnswerPacketID
func teoApiAnswerPacketID() C.int { return C.int(teonet.PacketIDAnswer) }

//export teoApiAnswerNo
func teoApiAnswerNo() C.int { return C.int(teonet.NoAnswer) }

// teoApiSetAnswerMode set APIData command answer mode
//export teoApiSetAnswerMode
func teoApiSetAnswerMode(c_cmdApi C.int, c_answerMode C.int) {
	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}
	cmdApi.SetAnswerMode(teonet.APIanswerMode(c_answerMode))
}

// teoApiSetReader add teonet api reader. Reader shoud allocate output data
// by malloc function, it will be free by Teonet-C Library after reader
// return in
//export teoApiSetReader
func teoApiSetReader(c_cmdApi C.int, c_reader unsafe.Pointer) {

	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}

	api, ok := teoc.getTeoApi(cmdApi.c_api)
	if ok == 0 {
		return
	}

	cmdApi.SetReader(func(c *teonet.Channel, p *teonet.Packet, data []byte) bool {

		// Prepare params
		c_data := unsafe.Pointer(nil)
		if data != nil {
			c_data = unsafe.Pointer(&data[0])
		}
		var c_data_len = C.int(len(data))
		c_addr := C.CString(c.Address())

		// Execute API server reader callback
		var c_outdata_len C.int
		c_outData := C.runAPIsReaderCb(C.c_api_s_reader(c_reader), c_cmdApi,
			c_addr, c_data, c_data_len, nil, &c_outdata_len,
		)
		C.free(unsafe.Pointer(c_addr))

		// Send API server answer
		outData := C.GoBytes(c_outData, c_outdata_len)
		api.SendAnswer(cmdApi, c, outData, p)

		// Free output data memory
		if c_outData != nil {
			C.free(c_outData)
		}

		return true
	})
}

// teoApiAdd add api command to API interface
//export teoApiAdd
func teoApiAdd(c_cmdApi C.int) {

	cmdApi, ok := teoc.getTeoApiCmd(c_cmdApi)
	if ok == 0 {
		return
	}

	api, ok := teoc.getTeoApi(cmdApi.c_api)
	if ok == 0 {
		return
	}

	api.Add(cmdApi)
}

// teoAddApiReader add API reader to teonet
//export teoAddApiReader
func teoAddApiReader(c_teo C.int, api_c C.int) {

	teo, ok := teoc.getTeo(c_teo)
	if ok == 0 {
		return
	}

	api, ok := teoc.getTeoApi(api_c)
	if ok == 0 {
		return
	}

	teo.AddReader(api.Reader())
}
