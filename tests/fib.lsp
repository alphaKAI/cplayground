(def-fun fib (n)
  (if (< n 2)
    n
    (+ (fib (- n 2)) (fib (- n 1)))))

(println (fib 10))
