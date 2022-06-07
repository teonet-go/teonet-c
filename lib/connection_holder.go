package main

import "C"
import (
	"sync"

	"github.com/teonet-go/teonet"
)

// Create teonet-c connections holder
func newTeonetC() *teonetC {
	return &teonetC{m: make(map[C.int]interface{})}
}

var teoc = newTeonetC()

// teonetC store teonet connections in map
type teonetC struct {
	m   map[C.int]interface{}
	key C.int
	sync.RWMutex
}

// add teonet connection or teoent api interface and return connection key
func (t *teonetC) add(teo interface{}) C.int {
	t.Lock()
	defer t.Unlock()

	t.key += 1
	t.m[t.key] = teo

	return t.key
}

// get teonet connection by key
func (t *teonetC) getTeo(teoKey C.int) (teo *teonet.Teonet, c_ok C.uchar) {
	t.RLock()
	defer t.RUnlock()

	i, ok := t.m[teoKey]
	if !ok {
		// c_ok = 0
		return
	}
	switch v := i.(type) {
	case *teonet.Teonet:
		c_ok = 1
		teo = v
	default:
		// c_ok = 0
	}
	return
}

// get teonet api client interface by key
func (t *teonetC) getTeoApiCli(key C.int) (teo *teonet.APIClient, c_ok C.uchar) {
	t.RLock()
	defer t.RUnlock()

	i, ok := t.m[key]
	if !ok {
		// c_ok = 0
		return
	}
	switch v := i.(type) {
	case *teonet.APIClient:
		c_ok = 1
		teo = v
	default:
		// c_ok = 0
	}
	return
}

// get teonet api interface by key
func (t *teonetC) getTeoApi(key C.int) (teo *teonet.API, c_ok C.uchar) {
	t.RLock()
	defer t.RUnlock()

	i, ok := t.m[key]
	if !ok {
		// c_ok = 0
		return
	}
	switch v := i.(type) {
	case *teonet.API:
		c_ok = 1
		teo = v
	default:
		// c_ok = 0
	}
	return
}

// get teonet api command data by key
func (t *teonetC) getTeoApiCmd(key C.int) (teo *cmdApiType, c_ok C.uchar) {
	t.RLock()
	defer t.RUnlock()

	i, ok := t.m[key]
	if !ok {
		// c_ok = 0
		return
	}
	switch v := i.(type) {
	case *cmdApiType:
		c_ok = 1
		teo = v
	default:
		// c_ok = 0
	}
	return
}
