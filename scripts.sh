#/bin/sh

watch_test() {
  while true; do
    inotifywait -e modify -e move -e create -e delete ./src ./test;
    npm install
    npm test
  done
}
