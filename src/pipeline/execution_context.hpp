#pragma once

template<typename TInput, typename TOutput>
class execution_context
{
private:
	TInput	_input;
	TOutput _output;

public:
	execution_context(TInput input) : _input(input), _output() {}

	TInput	get_input() const noexcept { return _input; }
	TOutput get_output() const noexcept { return _output; }
	void	set_output(TOutput value) noexcept { _output = value; }
};