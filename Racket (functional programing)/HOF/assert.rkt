#lang racket

; ------------------------------------------------------------------------------
; simple assert macros for testing
; peckato1-2019/11/10

; ------------------------------------------------------------------------------
; support for __LINE__ macro
; https://www.greghendershott.com/2014/06/file-and-line-in-racket.html

(require (for-syntax racket/base))
(define-syntax (__LINE__ stx)
  ;; `stx` comes from where the macro was invoked, so give _that_ to
  ;; `syntax-line`.
  (with-syntax ([line (syntax-line stx)])
    (syntax-case stx ()
      [_ #'line])))
; ------------------------------------------------------------------------------

(require compatibility/defmacro)

(define-macro (aux-assert expr) expr)

(define-macro (assert-true expr)
  `(unless (aux-assert ,expr)
       (error (format "Failed assertion (L~a): '(assert-true ~a)'" (syntax-line #'here) ',expr))))

(define-macro (assert-false expr)
  `(unless (not (aux-assert ,expr))
       (error (format "Failed assertion (L~a): '(assert-false ~a)'" (syntax-line #'here) ',expr))))

(define-macro (assert-equal lhs rhs)
  `(unless (aux-assert (equal? ,lhs ,rhs))
       (error (format "Failed assertion (L~a): '(assert-equal ~a ~a)'\n lhs: ~a\n rhs: ~a" (syntax-line #'here) ',lhs ',rhs ,lhs ,rhs))))


(provide (all-defined-out))
