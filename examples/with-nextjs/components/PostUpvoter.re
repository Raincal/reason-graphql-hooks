%raw
"require('../styles/main.css')";

open GraphqlHooksMutation;
open Util.ReactStuff;

module UpdatePost = [%graphql
  {|
    mutation updatePost($id: ID!, $votes: Int) {
      updatePost(id: $id, votes: $votes) {
        id
        __typename
        votes
      }
    }
  |}
];

[@react.component]
let make = (~id: string, ~votes: int, ~onUpdate: unit => unit) => {
  let (_, updatePost) =
    useMutation(
      ~request=
        UpdatePost.make(
          ~id,
          ~votes={
            votes + 1;
          },
          (),
        ),
      (),
    );

  <button
    className="votes--button"
    onClick={_ =>
      updatePost()
      |> Js.Promise.then_(_result => onUpdate() |> Js.Promise.resolve)
      |> ignore
    }>
    {votes->string_of_int->s}
  </button>;
};