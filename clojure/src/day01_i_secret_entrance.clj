(ns day01-i-secret-entrance
  (:require [clojure.string :refer [split-lines]])
  (:gen-class))

(def ^:const starting-position 50)
(def ^:const dial-size 100)

(defn parse-direction [line]
  {:pre [(contains? #{\L \R} (first line))]}
  (let [direction (first line)
        magnitude (Integer/parseInt (subs line 1))]
    (if (= direction \L)
      (- magnitude)
      magnitude)))

(defn read-directions [filename]
  (->> (slurp filename)
       split-lines
       (mapv parse-direction)))

(defn count-zero-positions [directions]
  (let [positions (reductions (fn [pos delta] (mod (+ pos delta) dial-size))
                              starting-position
                              directions)]
    (count (filter zero? (rest positions)))))

(defn -main [& args]
  (if-let [filename (first args)]
    (let [directions (read-directions filename)
          answer     (count-zero-positions directions)]
      (println "Password:" answer))
    (do
      (println "Usage:\n    clojure -M -m day01-i-secret-entrance <filename>")
      (System/exit 1))))