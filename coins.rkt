#lang racket

(define coins (list 2 3 5 7 9))

(define (check-formula lst)
  (define current-permutation (car lst))
  (unless (null? current-permutation)
    (let ([a (first current-permutation)]
          [b (second current-permutation)]
          [c (third current-permutation)]
          [d (fourth current-permutation)]
          [e (fifth current-permutation)])
      (if (= 399 (+ a (* b (expt c 2)) (expt d 3) (- e)))
          (display current-permutation)
          (check-formula (cdr lst))))))

(time (check-formula (permutations coins)))
