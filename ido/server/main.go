package main

import (
	"fmt"
	"html/template"
	"io"
	"log"
	"net/http"
	"strconv"
	"strings"
	"time"

	"github.com/jacobsa/go-serial/serial"
)

var port io.ReadWriteCloser

func main() {
	options := serial.OpenOptions{
		PortName:        "/dev/tty.HC-06-DevB",
		BaudRate:        9600,
		DataBits:        8,
		StopBits:        1,
		MinimumReadSize: 4,
	}
	var err error
	port, err = serial.Open(options)
	if err != nil {
		panic(err)
	}

	defer port.Close()

	http.HandleFunc("/", index)
	log.Println("server ready")
	http.ListenAndServe(":5959", nil)
}

type Data struct {
	Soil int
}

func index(w http.ResponseWriter, r *http.Request) {
	data := Data{}
	data.Soil = getSoil()
	generateHTML(w, data, "main")
}
func zero(w http.ResponseWriter, r *http.Request) {
	idoTime = 0
	w.Write([]byte("ok"))
	w.WriteHeader(200)
}

func gogo(w http.ResponseWriter, r *http.Request) {
	toggleTime(48)
	generateHTML(w, "gogo", "button")
}

func stop(w http.ResponseWriter, r *http.Request) {
	toggleTime(57)
	generateHTML(w, "stop", "button")
}

func generateHTML(w http.ResponseWriter, content interface{}, fileName ...string) {
	var files []string
	for _, file := range fileName {
		files = append(files, fmt.Sprintf("tmpl/%s.html", file))
	}
	templates := template.Must(template.ParseFiles(files...))

	err := templates.ExecuteTemplate(w, "layout", content)
	if err != nil {
		log.Println(err)
	}
}

func toggleTime(x byte) {
	b := []byte{x}
	_, _ = port.Write(b)
}

func getSoil() int {
	b := []byte{53}
	n, err := port.Write(b)
	log.Printf("write %d bytes\n", n)
	if err != nil {
		log.Printf("port.Write: %v\n", err)
	}
	time.Sleep(time.Millisecond * 700)

	r := make([]byte, 32)
	log.Println("before reading")
	n, err = port.Read(r)
	log.Printf("read %d bytes\n", n)
	if err != nil {
		log.Printf("port.Read: %v\n", err)
	}
	rNum := make([]byte, 0, 4)
	for _, x := range r {
		if x < 48 || x > 57 {
			fmt.Println("not number:", x)
			continue
		}
		rNum = append(rNum, x)
	}

	rs := strings.ReplaceAll(string(rNum), "\n", "")
	rs = strings.ReplaceAll(rs, "\r", "")
	/*
		rs = strings.TrimSpace(rs)
		rs = strings.ReplaceAll(rs, " ", "")
		rs = strings.ReplaceAll(rs, "\n", "")
	*/
	fmt.Println("len:", len(string(rNum)), rs, "...")
	value, err := strconv.Atoi(rs)
	if err != nil {
		log.Println("atoi:", err)
	}
	return value
}
