// Copyright 2021 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
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
// void safe_printf();
//
import "C"
import (
	"sync"
	"unsafe"

	"github.com/kirill-scherba/teonet"
)

func main() {

}

// Create teonet-c connections holder
func newTeonetC() *teonetC {
	return &teonetC{m: make(map[C.int]*teonet.Teonet)}
}

var teoc = newTeonetC()

// teonetC store teonet connections in map
type teonetC struct {
	m   map[C.int]*teonet.Teonet
	key C.int
	sync.RWMutex
}

// add teonet connection and return connection key
func (t *teonetC) add(teo *teonet.Teonet) C.int {
	t.Lock()
	defer t.Unlock()
	t.key += 1
	t.m[t.key] = teo

	return t.key
}

// get teonet connection by key
func (t *teonetC) get(teoKey C.int) (teo *teonet.Teonet, c_ok C.uchar) {
	teo, ok := t.m[teoKey]
	if ok {
		c_ok = 1
	}
	return
}

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

// teoNew start teonet client, return teo pointer to Teonet
//export teoNew
func teoNew(c_appShort *C.char) (teoKey C.int) {
	appShort := C.GoString(c_appShort)
	teo, err := teonet.New(appShort)
	/* params.appShort, params.port, reader, teonet.Log(), "NONE",
	params.showTrudp, params.logLevel, teonet.LogFilterT(params.logFilter */
	if err != nil {
		teo.Log().Println("can't init Teonet, error:", err)
		return
	}
	return teoc.add(teo)
}

// teoConnect connect to teonet, return true if ok
//export teoConnect
func teoConnect(c_teo C.int) (ok C.uchar) {
	teo, ok := teoc.get(c_teo)
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
	teo, ok := teoc.get(c_teo)
	if ok == 0 {
		return
	}
	address := teo.Address()
	return C.CString(address)
}

// teoConnectTo connect to teonet peer, return true if ok
//export teoConnectTo
func teoConnectTo(c_teo C.int, c_address *C.char) (ok C.uchar) {
	teo, ok := teoc.get(c_teo)
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

	teo, ok := teoc.get(c_teo)
	if ok == 0 {
		return
	}
	address := C.GoString(c_address)
	err := teo.ConnectTo(address, func(c *teonet.Channel, p *teonet.Packet, e *teonet.Event) (processed bool) {
		if e.Event != teonet.EventData {
			return
		}
		data := p.Data()
		addr := C.CString(c.Address())
		if C.runReaderCb(C.c_reader(c_reader),
			c_teo,
			addr,
			unsafe.Pointer(&data[0]),
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
func teoSendTo(c_teo C.int, c_address *C.char, c_data unsafe.Pointer, c_data_len C.int) (ok C.uchar) {
	teo, ok := teoc.get(c_teo)
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

//export teoWaitForever
func teoWaitForever(c_teo C.int) {
	select {}
}
